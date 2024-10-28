#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../headers/ast.h"
#include "../headers/info.h"
#include "../headers/utils.h"
#include "../headers/quadruple.h"

void _print_tree(AST* tree, int level);
void _pretty_print_tree(AST* tree, int level, char* prefix, char* connection, bool father_is_last, bool is_right);
bool _check_fn(InfoFunction* info, AST* tree, nat* count_returns);

AST* build_root(AST* left, Info* root_info, Tag tag, AST* right){
    return join_trees(left, new_node(root_info, tag), right);
}

AST* new_node(Info* info, Tag tag){
    AST* root = (AST*)malloc(sizeof(AST));
    root->tag = tag;
    root->info = info;
    root->left = NULL;
    root->right = NULL;
    return root;
}

AST* join_trees(AST* left, AST* root, AST* right){
    root->left = left;
    root->right = right;
    return root;
}

void free_tree(AST* tree){
    if(tree == NULL) return;

    free_tree(tree->left);
    free_tree(tree->right);
    free(tree);
}

void print_tree(AST* tree){
    if (tree == NULL) return;

    // puts("_print_tree");
    // _print_tree(tree, 0);

    char* prefix = malloc(sizeof(char));
    char* connection = malloc(sizeof(char));
    prefix[0] = '\0';
    connection[0] = '\0';

    // puts("\n_pretty_print_tree");
    _pretty_print_tree(tree, 0, prefix, connection, true, false);

    free(prefix);
    free(connection);
}

void _print_tree(AST* tree, int level){
    if(tree == NULL) return;
    printf("%s%i", repeat_str("   ", level), level);
    fflush(stdout);
    print_info(tree->tag, tree->info);
    _print_tree(tree->left, level+1);
    _print_tree(tree->right, level+1);
}

void _pretty_print_tree(AST* tree, int level, char* prefix, char* conection, bool father_is_last, bool is_right){
    if (tree == NULL) return;

    char* child_prefix = malloc(sizeof(char) * (strlen(prefix) + 12));
    char* left_child_connection = malloc(sizeof(char) * 12);
    char* right_child_connection = malloc(sizeof(char) * 12);
    child_prefix[0] = '\0';
    left_child_connection[0] = '\0';
    right_child_connection[0] = '\0';

    /* print info */
    printf("%s%s%i%s ",prefix, conection, level, level == 0 ? "" : is_right ? "r" : "l");
    print_node(tree);

    /* set prefix for children */
    if (level >= 1) {
        // need prefix
        if (father_is_last)
            sprintf(child_prefix, "%s%s", prefix, "    ");
        else
            sprintf(child_prefix, "%s%s", prefix, "│   ");
    }

    /* set connection for children */
    if (tree->left != NULL) {
        if (tree->right != NULL){
            // left child has a sibling
            sprintf(left_child_connection, "├── ");
        } else {
            // left child is alone (is the last)
            sprintf(left_child_connection, "└── ");
        }
    }

    if (tree->right != NULL) {
        // right child is (always) the last
        sprintf(right_child_connection, "└── ");
    }

    /* print children */
    _pretty_print_tree(tree->left, level+1, child_prefix, left_child_connection, tree->right == NULL, false);
    _pretty_print_tree(tree->right, level+1, child_prefix, right_child_connection, true, true);

    free(child_prefix);
    free(left_child_connection);
    free(right_child_connection);
    // "├── " "└── " "│   " "    "
}

void print_node(AST* node) {
    printf(YELLOW "%s " RESET, tag_to_str(node->tag));
    print_info(node->tag, node->info);
}

int temp_counter = 0; 

char* getTempName() {
    char* temp_name = (char*)malloc(10 * sizeof(char));
    sprintf(temp_name, "t%d", temp_counter++);
    return temp_name;
}

int label_counter = 0; 

char* getLabelName() {
    char* label_name = (char*)malloc(10 * sizeof(char));
    sprintf(label_name, "l%d", label_counter++);
    return label_name;
}

void generate_intermediate_code(AST* tree) {
    if (tree == NULL) {
        return;
    }

    Info* arg1 = NULL;
    Info* arg2 = NULL;
    Info* result = NULL;
    Tag tag = tree->tag;

    switch (tag) {
        case ADD:
        case SUB:
        case MUL:
        case DIV:
        case LESS:
        case GREATER:
        case EQUAL:
        case AND:
        case OR: {
            // Primero genero código intermedio para los hijos (posorder).
            generate_intermediate_code(tree->left);
            generate_intermediate_code(tree->right);

            if (tree->left != NULL && tree->right != NULL) {
                arg1 = tree->left->info;
                arg2 = tree->right->info;

                as_info_base(tree->info)->props->name = getTempName();
                result = tree->info;

                Quadruple* quad = new_quadruple(tag, arg1, arg2, result);
                print_quadruple(quad); 
            }
            break;
        }

        case NOT: {
            generate_intermediate_code(tree->right);

            if (tree->right != NULL) {
                arg1 = tree->right->info;
                
                as_info_base(tree->info)->props->name = getTempName();
                result = tree->info;

                Quadruple* quad = new_quadruple(tag, arg1, NULL, result);
                print_quadruple(quad); 
            }
            break;
        }

        case ASIG: 
        case DEC: {
            generate_intermediate_code(tree->left);
            generate_intermediate_code(tree->right);

            if (tree->left != NULL && tree->right != NULL) {
                arg1 = tree->right->info;
                result = tree->left->info; 

                Quadruple* quad = new_quadruple(ASIG, arg1, NULL, result);
                print_quadruple(quad);
            }
            break;
        }

        case RET: {
            generate_intermediate_code(tree->right);

            if (tree->right != NULL) {
                result = tree->right->info;
                Quadruple* quad = new_quadruple(tag, NULL, NULL, result);
                print_quadruple(quad); 
            }
            break;
        }

        case WHILE: {
            generate_intermediate_code(tree->left); // Genero código intermedio para la condición
            
            Info* start; 
            Info* end = NULL; // TODO: Encontrar la siguiente instruccion una vez finalizado el while

            as_info_base(tree->info)->props->name = getLabelName();
            start = tree->info;
            Quadruple* startLabel = new_quadruple(LABEL, NULL, NULL, start); // Etiqueta de inicio del while
            print_quadruple(startLabel);

            if (tree->left != NULL) {
                arg1 = tree->left->info; 
                Quadruple* condition = new_quadruple(IFF, arg1, NULL, end); // Si la condición evalua verdadera saltamos a endLabel
                print_quadruple(condition);
            }

            generate_intermediate_code(tree->right); // Genero código intermedio para el cuerpo del while

            Quadruple* jumpToStart = new_quadruple(JUMP, NULL, NULL, start); // Salto al inicio del while
            print_quadruple(jumpToStart);
            Quadruple* endLabel = new_quadruple(LABEL, NULL, NULL, end); // Etiqueta de fin del while
            print_quadruple(endLabel);

            break;
        }

        case IF: {
            generate_intermediate_code(tree->left); // Genero código intermedio para la condición

            Info* end = NULL; // TODO: Encontrar la siguiente instruccion una vez finalizado el if

            /* TODO: Conectar este Info* con el Info* del block 
            asignado como result en elseLabel (Ver case de T_E) */ 
            Info* _else = NULL; 

            if (tree->left != NULL) {
                arg1 = tree->left->info;
                /* Aqui se dan dos casos, si es solo if-then entonces el destino 
                en caso de que la condicion evalúe falsa será endLabel. Pero si es
                if-then-else el destino será elseLabel */
                Info* next = tree->left->tag != T_E ? end : _else;
                Quadruple* condition = new_quadruple(IFF, arg1, NULL, next); 
                print_quadruple(condition);
            }

            generate_intermediate_code(tree->right); // Genero código intermedio para el cuerpo del if

            Quadruple* endLabel = new_quadruple(LABEL, NULL, NULL, end); // Etiqueta de fin del if
            print_quadruple(endLabel);

            break;
        }

        case T_E: {
            generate_intermediate_code(tree->left); // Genero código intermedio para el then

            Info* end = NULL; // TODO: Conectar este Info* con el end del if (Ver case de IF)

            Quadruple* jumpToEnd = new_quadruple(JUMP, NULL, NULL, end); // Salto al inicio del while
            print_quadruple(jumpToEnd);

            if (tree->right != NULL) {
                as_info_base(tree->right->info)->props->name = getLabelName();
                result = tree->right->info; 
                Quadruple* elseLabel = new_quadruple(LABEL, NULL, NULL, result); // Ponemos un label para saber donde empieza el else
                print_quadruple(elseLabel);
            }

            generate_intermediate_code(tree->right);

            break;
        }

        default:
            generate_intermediate_code(tree->left);
            generate_intermediate_code(tree->right);
            break;
    }
}
