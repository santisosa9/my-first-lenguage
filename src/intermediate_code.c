#include <stdio.h>
#include <stdlib.h>
#include "../headers/ast.h"
#include "../headers/info.h"
#include "../headers/intermediate_code.h"
#include "../headers/quadruple.h"

void generate_intermediate_code_fn(AST* tree, LinkedList* ic);

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

void generate_intermediate_code_fn(AST* tree, LinkedList* ic) {
    if (ic == NULL) {
        fprintf(stderr, "Error: generate_intermediate_code: Invalid call error.\n");
        fprintf(stderr, "Precondition fault: 'ic' must not be NULL.\n");
        exit(EXIT_FAILURE);
    }

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
            generate_intermediate_code_fn(tree->left, ic);
            generate_intermediate_code_fn(tree->right, ic);

            if (tree->left != NULL && tree->right != NULL) {
                arg1 = tree->left->info;
                arg2 = tree->right->info;

                as_info_base(tree->info)->props->name = getTempName();
                result = tree->info;

                Quadruple* quad = new_quadruple(tag, arg1, arg2, result);
                insert_ll(ic, quad);
            }
            break;
        }

        case NOT: {
            generate_intermediate_code_fn(tree->right, ic);

            if (tree->right != NULL) {
                arg1 = tree->right->info;

                as_info_base(tree->info)->props->name = getTempName();
                result = tree->info;

                Quadruple* quad = new_quadruple(tag, arg1, NULL, result);
                insert_ll(ic, quad);
            }
            break;
        }

        case ASIG: {
            generate_intermediate_code_fn(tree->left, ic);
            generate_intermediate_code_fn(tree->right, ic);

            if (tree->left != NULL && tree->right != NULL) {
                arg1 = tree->right->info;
                result = tree->left->info;

                Quadruple* quad = new_quadruple(ASIG, arg1, NULL, result);
                insert_ll(ic, quad);
            }

            break;
        }

        case DEC: {
            generate_intermediate_code_fn(tree->left, ic);
            generate_intermediate_code_fn(tree->right, ic);

            if (tree->left != NULL && tree->right != NULL) {
              arg1 = tree->right->info;
              result = tree->left->info;

              Quadruple* quad = new_quadruple(ASIG, arg1, NULL, result);
              insert_ll(ic, quad);
            }
            
            break;
        }

        case RET: {
            generate_intermediate_code_fn(tree->right, ic);

            if (tree->right != NULL) {
                result = tree->right->info;
                Quadruple* quad = new_quadruple(tag, NULL, NULL, result);
                insert_ll(ic, quad);
            }
            break;
        }

        case WHILE: {

            char* startLabelName = getLabelName();
            char* endLabelName = getLabelName();

            Info* start;
            Info* end;

            as_info_base(tree->info)->props->name = startLabelName;
            start = tree->info;

            Quadruple* startLabel = new_quadruple(LABEL, NULL, NULL, start); // Etiqueta de inicio del while
            insert_ll(ic,  startLabel);

            generate_intermediate_code_fn(tree->left, ic); // Genero código intermedio para la condición

            end = new_info(NO_TYPED, 0, endLabelName, 0, 0);

            if (tree->left != NULL) {
                arg1 = tree->left->info;
                Quadruple* condition = new_quadruple(IFNOT, arg1, NULL, end); // Si la condición evalua falsa saltamos a endLabel
                insert_ll(ic, condition);
            }

            generate_intermediate_code_fn(tree->right, ic); // Genero código intermedio para el cuerpo del while

            Quadruple* jumpToStart = new_quadruple(JUMP, NULL, NULL, start); // Salto al inicio del while
            insert_ll(ic, jumpToStart);
            Quadruple* endLabel = new_quadruple(LABEL, NULL, NULL, end); // Etiqueta de fin del while
            insert_ll(ic, endLabel);
            break;
        }

        case IF: {
            generate_intermediate_code_fn(tree->left, ic); // Genero código intermedio para la condición

            char* nextLabelName = getLabelName();
            Info* next;

            if (tree->left != NULL) {
                arg1 = tree->left->info;
                next = new_info(NO_TYPED, 0, nextLabelName, 0, 0);
                Quadruple* condition = new_quadruple(IFNOT, arg1, NULL, next);
                insert_ll(ic, condition);
            }

            if (tree->right->tag != T_E) {
                generate_intermediate_code_fn(tree->right, ic);
                Quadruple* endLabel = new_quadruple(LABEL, NULL, NULL, next); // Etiqueta de fin del if
                insert_ll(ic, endLabel);
            } else {
                char* endLabelName = getLabelName();
                Info* end = new_info(NO_TYPED, 0, endLabelName, 0, 0);
                generate_intermediate_code_fn(tree->right->left, ic); // Bloque del then
                Quadruple* jumpToEnd = new_quadruple(JUMP, NULL, NULL, end); // Una vez terminado el then salto al fin del if
                insert_ll(ic, jumpToEnd);
                Quadruple* elseLabel = new_quadruple(LABEL, NULL, NULL, next); // Etiqueta del else
                insert_ll(ic, elseLabel);
                generate_intermediate_code_fn(tree->right->right, ic); // Bloque del else
                Quadruple* endLabel = new_quadruple(LABEL, NULL, NULL, end); // Etiqueta de fin del if
                insert_ll(ic, endLabel);
            }

            break;
        }

        case FN_DEC: {
          Quadruple* dec = new_quadruple(tree->tag, NULL, NULL, tree->info);
          insert_ll(ic, dec);
          generate_intermediate_code_fn(tree->right, ic);
          Quadruple* end = new_quadruple(FN_END, NULL, NULL, tree->info);
          insert_ll(ic, end);

          break;
        }

        case FN_CALL: {
            // Generate intermediate code for the parameters
            generate_intermediate_code_fn(tree->right, ic);

            // Append the generated parameters to the function call
            AST* current = tree->right;
            nat i = 1;
            while (current != NULL)
            {
                AST* expr = current->left;
                if (expr != NULL) {
                    Info* num_param = new_info(NO_TYPED, i, itos(i), 0, 0);
                    Quadruple* param = new_quadruple(PARAMETER, num_param, NULL, expr->info);
                    insert_ll(ic, param);
                }
                current = current->right;
                i++;
            }

            // Generate intermediate code for the function call
            arg1 = new_info(NO_TYPED, 0, as_info_fn(tree->info)->props->name, 0, 0);
            char* name = (char*)malloc(10 * sizeof(char));
            if (name != NULL) {
                sprintf(name, "%d", as_info_fn(tree->info)->cant_params);
            }
            arg2 = new_info(NO_TYPED, 0, name, 0, 0);
            as_info_fn(tree->info)->props->name = getTempName();
            Quadruple* call;
            if (as_info_fn(tree->info)->props->type != VOID) {
              result = tree->info;
              call = new_quadruple(TYPED_FC, arg1, arg2, result);
            } else {
              call = new_quadruple(VOID_FC, arg1, arg2, NULL);
            }
            insert_ll(ic, call);
            break;
        }

        default:
            generate_intermediate_code_fn(tree->left, ic);
            generate_intermediate_code_fn(tree->right, ic);
            break;
    }
}

void generate_intermediate_code(AST* tree, LinkedList* ic) {
    if (ic == NULL) {
        fprintf(stderr, "Error: generate_intermediate_code: Invalid call error.\n");
        fprintf(stderr, "Precondition fault: 'ic' must not be NULL.\n");
        exit(EXIT_FAILURE);
    }

    if (tree == NULL) {
        return;
    }

    Info* result = NULL;
    Tag tag = tree->tag;

    switch (tag) {
        case DEC: {
            generate_intermediate_code(tree->left, ic);
            generate_intermediate_code(tree->right, ic);

            result = tree->info;
            Quadruple* global_quad = new_quadruple(GLOBAL_DEC, NULL, NULL, result); 
            insert_head(ic, global_quad);
          
            break;
        }

        case FN_DEC: {
            if (!as_info_fn(tree->info)->is_extern) {
                Quadruple* dec = new_quadruple(tree->tag, NULL, NULL, tree->info);
                insert_ll(ic, dec);
                generate_intermediate_code_fn(tree->right, ic);
                Quadruple* end = new_quadruple(FN_END, NULL, NULL, tree->info);
                insert_ll(ic, end);
            }

            break;
        }

        default:
            generate_intermediate_code(tree->left, ic);
            generate_intermediate_code(tree->right, ic);
            break;
    }
}
