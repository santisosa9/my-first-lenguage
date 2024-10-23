#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../headers/symbol_table.h"
#include "../headers/utils.h"
#include "../headers/ast.h"

Info* _search_in_scope(STStackNode* scope, char* name);
bool _check_fn(InfoFunction* info, AST* tree, nat* count_returns);
void insert_params(SymbolTable* table, Info* info);

SymbolTable* new_symbol_table(){
    SymbolTable* table = (SymbolTable*)malloc(sizeof(SymbolTable));
    table->scope_size = 1; // the global scope
    table->total_size = 0;
    table->base = new_symbol_table_stack_node(NULL, NULL, NULL);
    table->top = table->base;
    return table;
}

STStackNode* new_symbol_table_stack_node(STListNode* head, STStackNode* next, STStackNode* prev){
    STStackNode* node = (STStackNode*)malloc(sizeof(STStackNode));
    node->head = head;
    node->next = next;
    node->prev = prev;
    return node;
}

STListNode* new_symbol_table_list_node(Info* info, STListNode* next){
    STListNode* node = (STListNode*)malloc(sizeof(STListNode));
    node->info = info;
    node->next = next;
    return node;
}

void open_scope(SymbolTable* table){
    STStackNode* new_node = new_symbol_table_stack_node(NULL, NULL, table->top);
    table->top->next = new_node;
    table->top = new_node;
    table->scope_size++;
}

void close_scope(SymbolTable* table) {
    if (table->scope_size <= 1) return;

    STStackNode* prev = table->top->prev;

    free_symbol_table_stack_node(table->top);
    table->top = prev;
    table->scope_size--;
}

bool current_scope_is_global(SymbolTable* table) {
    return table->top == table->base && table->scope_size == 1;
}

bool insert(SymbolTable* table, Info* info){
    if(table == NULL) return false;

    STListNode* new_node = new_symbol_table_list_node(info, table->top->head);
    table->top->head = new_node;
    table->total_size++;
    return true;
}

void insert_params(SymbolTable* table, Info* info) {
    LinkedList*params = as_info_fn(info)->params;
    LinkedListIterator* formal_params = new_linked_list_iterator(params);
    while (has_next(formal_params)) {
        Info* PARAM_SEC = (Info*)next(formal_params);
        insert(table, PARAM_SEC);
    }
}

Info* search(SymbolTable* table, char* name){
    if(table == NULL) return NULL;

    STStackNode* current_scope = table->top;

    while (current_scope != NULL) {
        Info* result = _search_in_scope(current_scope, name);
        if (result != NULL) {
            return result;
        }
        current_scope = current_scope->prev;
    }

    return NULL;
}

Info* search_in_current_scope(SymbolTable* table, char* name) {
    if (table == NULL) return NULL;
    if (table->top == NULL) return NULL;

    return _search_in_scope(table->top, name);
}

Info* search_global(SymbolTable* table, char* name) {
    if (table == NULL) return NULL;

    return _search_in_scope(table->base, name);
}

Info* _search_in_scope(STStackNode* scope, char* name) {
    if (scope == NULL) return NULL;

    STListNode* current = scope->head;

    while (current != NULL) {
        if (strcmp(as_info_base(current->info)->props->name, name) == 0) {
            return current->info;
        }
        current = current->next;
    }

    return NULL;
}

bool update(SymbolTable* table, AST* tree, Info* info){
    if (table == NULL) return false;
    if (tree == NULL) return false;

    Info* target = NULL;
    if (tree->tag == FN_DEC || tree->tag == FN_CALL)
        target = search_global(table, as_info_fn(info)->props->name);
    else
        target = search(table, as_info_base(info)->props->name);

    if(target == NULL) return false;

    copy_info(tree->tag, target, info);
    tree->info = target;

    return true;
}


Info* erase(SymbolTable* table, char* name){
    if(table == NULL) return NULL;

    STStackNode* current_scope = table->top;

    while (current_scope != NULL) {
        STListNode* current = current_scope->head;
        STListNode* prev = NULL;

        while (current != NULL) {
            if (strcmp(as_info_base(current->info)->props->name, name) == 0) {
                Info* info = current->info;
                if (prev == NULL) {
                    current_scope->head = current->next;
                } else {
                    prev->next = current->next;
                }
                free(current);
                return info;
            }
            prev = current;
            current = current->next;
        }

        current_scope = current_scope->prev;
    }

    return NULL;
}

void print_table(SymbolTable* table){
    if(table == NULL) return;

    STStackNode* current_scope = table->base;

    while (current_scope != NULL) {
        STListNode* current = current_scope->head;

        while (current != NULL) {
            Tag currentTag = current->info->fn.props->is_fn ? FN_DEC : ID;
            print_info(currentTag, current->info);
            current = current->next;
        }

        current_scope = current_scope->next;
    }
}

void free_table(SymbolTable* table){
    if (table == NULL) return;

    STStackNode* current_scope = table->top;
    STStackNode* prev = NULL;

    while (current_scope != NULL) {
        free_symbol_table_stack_node(current_scope);
        current_scope = prev;
    }

    free(table);
}

void free_symbol_table_stack_node(STStackNode* node){
    if(node == NULL) return;

    while (node->head != NULL)
    {
        STListNode* next = node->head->next;
        free_symbol_table_list_node(node->head);
        node->head = next;
    }

    free(node);
}

void free_symbol_table_list_node(STListNode* node){
    if(node == NULL) return;
    free(node);
}

bool decorate_tree(AST* tree, SymbolTable* table) {
    if (tree == NULL) return true;

    Tag currentTag = tree->tag;
    Info* existing;

    switch (currentTag) {
        case FN_DEC: {
            existing = search_global(table, as_info_fn(tree->info)->props->name);
            if (existing == NULL) {
                insert(table, tree->info);
                open_scope(table);
                insert_params(table, tree->info);
                if (!decorate_tree(tree->right, table)) return false;
                close_scope(table);
            } else {
                printf("Error: Redeclaración de la función '%s' en línea %d.\n",
                       as_info_fn(tree->info)->props->name,
                       as_info_fn(tree->info)->props->line);
                return false;
            }
            break;
        }

        case BLOCK: {
            open_scope(table);
            if (!decorate_tree(tree->right, table)) return false;
            close_scope(table);
            break;
        }

        case DEC: {
            existing = search_in_current_scope(table, as_info_base(tree->info)->props->name);
            if (existing == NULL) {
                insert(table, tree->info);
            } else {
                printf("Error: Redeclaración de la variable '%s' en línea %d.\n",
                    as_info_base(tree->info)->props->name,
                    as_info_base(tree->info)->props->line);
                return false;
            }
            if (!decorate_tree(tree->left, table) || !decorate_tree(tree->right, table)) return false;
            break;
        }

        case ID: {
            existing = search(table, as_info_base(tree->info)->props->name);
            if (existing == NULL) {
                printf("Error: Variable '%s' no declarada en línea %d.\n",
                    as_info_base(tree->info)->props->name,
                    as_info_base(tree->info)->props->line);
                return false;
            } else {
                update(table, tree, existing);
            }
            if (!decorate_tree(tree->left, table) || !decorate_tree(tree->right, table)) return false;
            break;
        }

        case FN_CALL: {
            existing = search_global(table, as_info_fn(tree->info)->props->name);
            if (existing == NULL) {
                printf("Error: Función '%s' no declarada en línea %d.\n",
                    as_info_fn(tree->info)->props->name,
                    as_info_fn(tree->info)->props->line);
                return false;
            } else {
                update(table, tree, existing);
            }
            if (!decorate_tree(tree->left, table) || !decorate_tree(tree->right, table)) return false;
            break;
        }

        default: {
            if (!decorate_tree(tree->left, table) || !decorate_tree(tree->right, table)) return false;
            break;
        }
    }

    return true;
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
            bool left_check = check_types(tree->left);
            bool right_check = check_types(tree->right);
            ok &= left_check && right_check;
            break;
    }

    return ok;
}
