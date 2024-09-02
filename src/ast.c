#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../headers/ast.h"
#include "../headers/utils.h"

void _print_tree(AST* tree, int level);
void _pretty_print_tree(AST* tree, int level, char* prefix, char* connection, bool rigth_was_last);


AST* build_root(AST* left, Info* root_info, AST* right){
    return join_trees(left, new_node(root_info), right);
}

AST* new_node(Info* info){
    AST* root = (AST*)malloc(sizeof(AST));
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

Info* new_info(Tag tag, Type type, int value, char* name, int line, int col){
    Info* info = (Info*)malloc(sizeof(Info));
    info->tag = tag;
    info->type = type;
    info->value = value;
    info->name = name;
    info->line = line;
    info->col = col;
    return info;
}

void free_tree(AST* tree){
    if(tree == NULL) {
        // puts("free_tree: tree is NULL");
        return;
    }
    // printf("free_tree: tree->info->name: %s\n", tree->info->name);
    // printf("free_tree: tree->left : %s\n", tree->left == NULL ? "NULL" : tree->left->info->name);
    // printf("free_tree: tree->right: %s\n", tree->left == NULL ? "NULL" : tree->right->info->name);
    free_tree(tree->left);
    free_tree(tree->right);
    free(tree->info);
    free(tree);
}

void print_tree(AST* tree){
    if (tree == NULL) return;
    puts("_print_tree");
    _print_tree(tree, 0);
    char* prefix = malloc(sizeof(char));
    prefix[0] = '\0';
    char* connection = malloc(sizeof(char));
    connection[0] = '\0';
    puts("\n_pretty_print_tree");
    _pretty_print_tree(tree, 0, prefix, connection, true);
    free(prefix);
}

void _print_tree(AST* tree, int level){
    if(tree == NULL) return;
    printf("%s%i", repeat_str("   ", level), level);
    fflush(stdout);
    print_info(tree->info);
    _print_tree(tree->left, level+1);
    _print_tree(tree->right, level+1);
}

void _pretty_print_tree(AST* tree, int level, char* prefix, char* conection, bool father_is_last){
    if (tree == NULL) return;

    char* child_prefix = malloc(sizeof(char) * (strlen(prefix) + 12));
    char* left_child_connection = malloc(sizeof(char) * 12);
    char* right_child_connection = malloc(sizeof(char) * 12);

    /* print info */
    printf("%s%s%i",prefix, conection, level);
    print_info(tree->info);

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
    _pretty_print_tree(tree->left, level+1, child_prefix, left_child_connection, tree->right == NULL);
    _pretty_print_tree(tree->right, level+1, child_prefix, right_child_connection, true);
}
// "├── " "└── " "│   " "    "


void print_info(Info* info){
    printf("{");
    printf("Tag: %s, ", tag_to_str(info->tag));
    printf("Type: %s, ", type_to_str(info->type));
    printf("Value: %s, ", value_to_str(info->value, info->type));
    printf("Name: %s, ", info->name);
    printf("Line: %d", info->line);
    printf("}\n");
}


char* tag_to_str(Tag tag){
    switch (tag){
        case ID:    return strdup("ID");            break;
        case MAIN:  return strdup("MAIN");          break;
        case RET:   return strdup("RET");           break;
        case STMT:  return strdup("STMT");          break;
        case ASIG:  return strdup("ASIG");          break;
        case TYPE:  return strdup("TYPE");          break;
        case COMMA: return strdup("COMMA");         break;
        case DEC:   return strdup("DEC");           break;
        case NOT:   return strdup("NOT");           break;
        case OR:    return strdup("OR");            break;
        case AND:   return strdup("AND");           break;
        case ADD:   return strdup("ADD");           break;
        case MUL:   return strdup("MUL");           break;
        case VALUE: return strdup("VALUE");         break;
        default:    return strdup("UNKNOWN_TAG");   break;
    }
}

char* type_to_str(Type type){
    switch (type){
        case INT:       return strdup("INT");           break;
        case FLOAT:     return strdup("FLOAT");         break;
        case CHAR:      return strdup("CHAR");          break;
        case STRING:    return strdup("STRING");        break;
        case BOOL:      return strdup("BOOL");          break;
        case VOID:      return strdup("VOID");          break;
        case NO_TYPED:  return strdup("NO_TYPED");      break;
        case ANY:       return strdup("ANY");           break;
        default:        return strdup("UNKNOWN_TYPE");  break;
    }
}

char* value_to_str(int value, Type type){
    switch (type) {
        case BOOL:  return value == 0 ? strdup("FALSE") : strdup("TRUE");
        default:    return itos(value);
    }
}
