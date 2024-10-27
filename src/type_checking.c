#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../headers/ast.h"
#include "../headers/utils.h"
#include "../headers/type_checking.h"

bool _check_fn(InfoFunction* info, AST* tree, nat* count_returns);

bool check_expr(AST* tree) {
    if (tree == NULL) {
        return true;
    }

    Tag tag = tree->tag;
    Type left_type = ANY;
    Type right_type = ANY;
    bool ok = true;

    switch (tag) {
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

        case DEC:{
            if (tree->right == NULL) {
                fprintf(stderr, "Warn: Variable '%s' declared at line %u has not an initial value.\n",
                    as_info_base(tree->info)->props->name,
                    as_info_base(tree->info)->props->line
                );
                ok &= true;
                break;
            }

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

        case FN_CALL: {
            ok &= check_params(as_info_fn(tree->info)->params, tree);
            break;
        }

        case ID:
        case VALUE: {
            ok &= true;
            break;
        }

        // Is not an expression
        default: {
            ok &= false;
            break;
        }
    }

    return ok;
}

bool check_fn(AST* fn) {
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
    AST* body = fn->right;

    if (body == NULL && !info->is_extern && info->props->type != VOID) {
        fprintf(stderr, "Error: in '%s' function should return a value of type '%s' but returns 'void' at line %u.\n",
            info->props->name,
            type_to_str(info->props->type),
            info->props->line
        );
        return false;
    }

    if (body != NULL && info->is_extern) {
        printf("Error: function '%s' declared as extern cannot have a body at line %u.\n",
            info->props->name,
            info->props->line
        );
        return false;
    }

    nat count_returns = 0;

    bool ok = _check_fn(info, body, &count_returns);

    if (count_returns < 1 && !info->is_extern && info->props->type != VOID) {
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

    if (tree == NULL) // Empty body function
        return true;

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

        case IF:
        case T_E:
        case BLOCK:
        case WHILE: {
            bool left_check = _check_fn(info, tree->left, count_returns);
            bool right_check = _check_fn(info, tree->right, count_returns);
            ok &= left_check && right_check;
            break;
        }

        default: {
            ok &= check_expr(tree);
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
        ok &= check_expr(expr);
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

bool check_types(AST* tree) {
    if (tree == NULL) {
        return true;
    }

    Tag tag = tree->tag;
    bool ok = true;

    switch (tag){
        case FN_DEC: {
            ok &= check_fn(tree);
            break;
        }

        // Outside FN_DEC:
        case BLOCK:
        case WHILE:
        case IF:
        case T_E:
        case PARAM_SEC:
        case PROGRAM:
        case RET :
        case SEMICOLON: {
            bool left_check = check_types(tree->left);
            bool right_check = check_types(tree->right);
            ok &= left_check && right_check;
            break;
        }

        case ID:
        case VALUE:
        case TYPE:{
            ok &= true;
            break;
        }

        // Expressions:
        default:{
            ok &= check_expr(tree);
            break;
        }
    }

    return ok;
}
