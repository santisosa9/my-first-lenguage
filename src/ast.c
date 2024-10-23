#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../headers/ast.h"
#include "../headers/info.h"
#include "../headers/utils.h"

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
    printf("Tag: %s, ", tag_to_str(node->tag));
    print_info(node->tag, node->info);
}

bool check_fn(AST* fn){
    if (fn == NULL) {
        fprintf(stderr, "Error: check_fn: Invalid call error.\n");
        fprintf(stderr, "Precondition fault: 'fn' must not be NULL.\n");
        exit(EXIT_FAILURE);
    }

    if (fn->tag != FN_DEC) {
        fprintf(stderr, "Error: check_fn: Invalid call error.\n");
        fprintf(stderr, "Precondition fault: 'fn' must be a FN_DEC node.\n");
        exit(EXIT_FAILURE);
    }

    InfoFunction* info = as_info_fn(fn->info);
    nat count_returns = 0;

    bool ok = _check_fn(info, fn->right, &count_returns);


    if (count_returns < 1 && info->props->type != VOID && !info->is_extern) {
        fprintf(stderr, "Error: function '%s' at line %u should return a value of type '%s' but never returns.\n",
            info->props->name,
            info->props->line,
            type_to_str(info->props->type)
        );
        ok &= false;
    }

    return ok;
}

bool _check_fn(InfoFunction* info, AST* tree, nat* count_returns) {
    Type return_type = info->props->type;

    if (info->is_extern && tree == NULL) return true;

    if (info->is_extern && tree != NULL) {
        printf("Error: function '%s' declared as extern cannot have a body at line %u.\n",
            info->props->name,
            info->props->line
        );
        return false;
    }

    if (tree == NULL) { // Empty body function
        if (return_type != VOID) {
            printf("Error: in '%s' function should return a value of type '%s' but returns 'void' at line %u.\n",
                info->props->name,
                type_to_str(return_type),
                info->props->line
            );
            return false;
        }
        return true;
    }

    Tag tag = tree->tag;
    Type expr_type;
    bool ok = true;

    switch (tag){
        case RET:{
            (*count_returns)++;

            if (return_type != VOID) {
                if (tree->right == NULL) {
                    printf("Error: in '%s' function should return a value of type '%s' but returns 'void' at line %u.\n",
                        info->props->name,
                        type_to_str(return_type),
                        as_info_base(tree->info)->props->line
                    );
                    ok &= false;
                }

                expr_type = as_info_base(tree->right->info)->props->type;

                if (expr_type != return_type) {
                    printf("Error: in '%s' function should return a value of type '%s' but returns '%s' at line %u.\n",
                        info->props->name,
                        type_to_str(return_type),
                        type_to_str(expr_type),
                        as_info_base(tree->info)->props->line
                    );
                    ok &= false;
                }
            }

            break;
        }

        case SEMICOLON: {
            if (tree->left != NULL && tree->left->tag == RET && tree->right != NULL) {
                printf("Warning: unaccecible code after line '%u'.\n",
                    as_info_base(tree->left->info)->props->line
                );
            }

            bool left_check = _check_fn(info, tree->left, count_returns);
            bool right_check = _check_fn(info, tree->right, count_returns);
            ok &= left_check && right_check;
            break;
        }
        default:{
            ok &= check_types(tree);
            break;
        }

    }

    return ok;

}

bool check_params(LinkedList* formal_params, AST* fn_call) {
    if (formal_params == NULL) {
        fprintf(stderr, "Error: check_params: Invalid call error.\n");
        fprintf(stderr, "Precondition fault: 'formal_params' must not be NULL.\n");
        exit(EXIT_FAILURE);
    }

    if (fn_call == NULL) {
        fprintf(stderr, "Error: check_params: Invalid call error.\n");
        fprintf(stderr, "Precondition fault: 'fn_call' must not be NULL.\n");
        exit(EXIT_FAILURE);
    }

    if (fn_call->tag != FN_CALL) {
        fprintf(stderr, "Error: check_params: Invalid call error.\n");
        fprintf(stderr, "Precondition fault: 'fn_call' must be a FN_CALL node.\n");
        exit(EXIT_FAILURE);
    }

    bool ok = true;

    LinkedListIterator* formal = new_linked_list_iterator(formal_params);
    AST* current_param = fn_call->right;

    while (has_next(formal) && current_param != NULL) {
        Info* info = (Info*)next(formal);
        Type formal_type = as_info_base(info)->props->type;
        AST* expr = current_param->left;
        ok &= check_types(expr);
        Type current_type = as_info_base(expr->info)->props->type;

        if (formal_type != current_type) {
            fprintf(stderr, "Error: Wrong argument type to function ‘%s’ at line %u. Argument ‘%s’ expects: '%s', found: '%s'.\n",
                as_info_fn(fn_call->info)->props->name,
                as_info_fn(fn_call->info)->props->line,
                as_info_base(info)->props->name,
                type_to_str(formal_type),
                type_to_str(current_type)
            );
            ok &= false;
        }

        current_param = current_param->right;
    }

    // Check param analisys exit conditions
    if (has_next(formal) && current_param == NULL) {
        fprintf(stderr, "Error: too few arguments to function ‘%s’ at line %u.\n", as_info_fn(fn_call->info)->props->name, as_info_base(fn_call->info)->props->line);
        ok &= false;
    }

    if (!has_next(formal) && current_param != NULL) {
        fprintf(stderr, "Error: too many arguments to function ‘%s’ at line %u.\n", as_info_fn(fn_call->info)->props->name, as_info_fn(fn_call->info)->props->line);
        ok &= false;
    }

    return ok;
}

bool check_types(AST* tree){
    if (tree == NULL) {
        return true;
    }

    Tag tag = tree->tag;
    Type left_type = ANY;
    Type right_type = ANY;
    bool ok = true;
    bool left_check;
    bool right_check;

    switch (tag){
        case ADD:
        case MUL:
        case SUB:
        case DIV: {
            left_type = as_info_base(tree->left->info)->props->type;
            right_type = as_info_base(tree->right->info)->props->type;
            if (left_type != INT) {
                fprintf(stderr, "Error: Wrong type for left operand of '%s' at line %u. Expected: '%s', found: '%s'.\n",
                    as_info_base(tree->info)->props->name,
                    as_info_base(tree->info)->props->line,
                    type_to_str(INT),
                    type_to_str(left_type)
                );
                ok &= false;
                break;
            }

            if (right_type != INT) {
                fprintf(stderr, "Error: Wrong type for right operand of '%s' at line %u. Expected: '%s', found: '%s'.\n",
                    as_info_base(tree->info)->props->name,
                    as_info_base(tree->info)->props->line,
                    type_to_str(INT),
                    type_to_str(right_type)
                );
                ok &= false;
                break;
            }


            bool left_check = check_types(tree->left);
            bool right_check = check_types(tree->right);
            ok &= left_check && right_check;
            break;
        }

        case LESS:
        case GREATER:
        case EQUAL: {
            left_type = as_info_base(tree->left->info)->props->type;
            right_type = as_info_base(tree->right->info)->props->type;
            if (left_type != right_type) {
                fprintf(stderr, "Error: Operands of '%s' must have the same type at line %u. Found: '%s' and '%s'.\n",
                    as_info_base(tree->info)->props->name,
                    as_info_base(tree->info)->props->line,
                    type_to_str(INT),
                    type_to_str(right_type)
                );
                ok &= false;
                break;
            }

            bool left_check = check_types(tree->left);
            bool right_check = check_types(tree->right);
            ok &= left_check && right_check;
            break;
        }

        case AND:
        case OR: {
            left_type = as_info_base(tree->left->info)->props->type;
            right_type = as_info_base(tree->right->info)->props->type;
            if (left_type != BOOL) {
                fprintf(stderr, "Error: Wrong type for left operand of '%s' at line %u. Expected: '%s', found: '%s'.\n",
                    as_info_base(tree->info)->props->name,
                    as_info_base(tree->info)->props->line,
                    type_to_str(BOOL),
                    type_to_str(left_type)
                );
                ok &= false;
                break;
            }

            if (right_type != BOOL) {
                fprintf(stderr, "Error: Wrong type for right operand of '%s' at line %u. Expected: '%s', found: '%s'.\n",
                    as_info_base(tree->info)->props->name,
                    as_info_base(tree->info)->props->line,
                    type_to_str(BOOL),
                    type_to_str(right_type)
                );
                ok &= false;
                break;
            }

            bool left_check = check_types(tree->left);
            bool right_check = check_types(tree->right);
            ok &= left_check && right_check;
            break;
        }

        case NOT: {
            right_type = as_info_base(tree->right->info)->props->type;
            if (right_type != BOOL) {
                fprintf(stderr, "Error: Wrong type for the operand of '%s' at line %u. Expected: '%s', found: '%s'.\n",
                    as_info_base(tree->info)->props->name,
                    as_info_base(tree->info)->props->line,
                    type_to_str(BOOL),
                    type_to_str(right_type)
                );
                ok &= false;
                break;
            }
            ok &= check_types(tree->right);
            break;
        }

        case ASIG: {
            left_type = as_info_base(tree->left->info)->props->type;
            right_type = as_info_base(tree->right->info)->props->type;
            if (left_type != right_type) {
                fprintf(stderr, "Error: Operands of '%s' must have the same type at line %u. Found: '%s' and '%s'.\n",
                    as_info_base(tree->info)->props->name,
                    as_info_base(tree->info)->props->line,
                    type_to_str(left_type),
                    type_to_str(right_type)
                );
                ok &= false;
                break;
            }
            ok &= check_types(tree->right);
            break;
        }

        case FN_DEC: {
            ok &= check_fn(tree);
            break;
        }

        case FN_CALL: {
            ok &= check_params(as_info_fn(tree->info)->params, tree);
            break;
        }

        // BLOCK, WHILE, IF, T_E, PARAM_SEC, PROGRAM, RET (outside FN_DEC), SEMICOLON (outside FN_DEC)
        default:
            left_check = check_types(tree->left);
            right_check = check_types(tree->right);
            ok &= left_check && right_check;
            break;
    }

    return ok;
}

