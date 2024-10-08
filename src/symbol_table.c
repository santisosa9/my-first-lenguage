#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../headers/symbol_table.h"
#include "../headers/utils.h"
#include "../headers/ast.h"

Type get_type_from_table(AST* node, SymbolTable* table);
Type get_type(char* name, SymbolTable* table);
Info* _search_in_scope(STStackNode* scope, char* name);

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

void close_scope(SymbolTable* table){
    if(table->scope_size <= 1) return;

    STStackNode* prev = table->top->prev;
    STListNode* current = table->top->head;
    STListNode* next = NULL;

    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }

    free_symbol_table_stack_node(table->top);
    table->top = prev;
    table->scope_size--;
}

bool insert(SymbolTable* table, Info* info){
    if(table == NULL) return false;

    STListNode* new_node = new_symbol_table_list_node(info, table->top->head);
    table->top->head = new_node;
    table->total_size++;
    return true;
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
    if(table == NULL) return false;
    if (tree == NULL) return false;

    Info* target = NULL;
    if (tree->tag == DEC_FN || tree->tag == CALL_FN)
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
            Tag currentTag = current->info->fn.props->is_fn ? DEC_FN : ID;
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

bool fill_table(AST* tree, SymbolTable* table) {
    if (tree == NULL) {
        return true;
    }

    Info* existing;
    Tag currentTag = tree->tag;

    switch (currentTag) {
        case DEC:
            existing = search(table, as_info_base(tree->info)->props->name);
            if (existing == NULL) {
                insert(table, tree->info);
            } else {
                printf("Error: Redeclaración de la variable '%s' en línea %d.\n", as_info_base(tree->info)->props->name, as_info_base(tree->info)->props->line);
                return false;
            }
            break;

        case DEC_FN:
            existing = search_global(table, as_info_fn(tree->info)->props->name);
            if (existing == NULL) {
                insert(table, tree->info);
            } else {
                printf("Error: Redeclaración de la función '%s' en línea %d.\n", as_info_fn(tree->info)->props->name, as_info_fn(tree->info)->props->line);
                return false;
            }
            break;

        default:
            break;
    }

    return fill_table(tree->left, table) && fill_table(tree->right, table);
}

bool check_types(AST* tree, SymbolTable* table) {
    if (tree == NULL) {
        return true;
    }

    Tag tag = tree->tag;
    Type left_type;
    Type right_type;

    switch (tag) {
        case ADD:
        case MUL: {
            left_type = get_type_from_table(tree->left, table);
            right_type = get_type_from_table(tree->right, table);
            if (left_type == INT && right_type == INT) {
                Info* updatedInfo = tree->info;
                as_info_base(updatedInfo)->props->type = INT;
                update(table, tree, updatedInfo);
            } else {
                printf("Error: Operación inválida para los tipos %s, %s en línea %d.\n", type_to_str(left_type), type_to_str(right_type), as_info_base(tree->info)->props->line);
                return false;
            }
            break;
        }

        case AND:
        case OR: {
            left_type = get_type_from_table(tree->left, table);
            right_type = get_type_from_table(tree->right, table);
            if (left_type == BOOL && right_type == BOOL) {
                Info* updatedInfo = tree->info;
                as_info_base(updatedInfo)->props->type = BOOL;
                update(table, tree, updatedInfo);
            } else {
                printf("Error: Operación inválida para los tipos %s, %s en línea %d.\n", type_to_str(left_type), type_to_str(right_type), as_info_base(tree->info)->props->line);
                return false;
            }
            break;
        }

        case EQUALS: {
            left_type = get_type_from_table(tree->left, table);
            right_type = get_type_from_table(tree->right, table);
            if ((left_type == INT && right_type == INT) || (left_type == BOOL && right_type == BOOL)) {
                Info* updatedInfo = tree->info;
                as_info_base(updatedInfo)->props->type = BOOL;
                update(table, tree, updatedInfo);
            } else {
                printf("Error: Operación inválida para los tipos %s, %s en línea %d.\n", type_to_str(left_type), type_to_str(right_type), as_info_base(tree->info)->props->line);
                return false;
            }
            break;
        }

        case MINOR:
        case BIGGER: {
            left_type = get_type_from_table(tree->left, table);
            right_type = get_type_from_table(tree->right, table);
            if (left_type == INT && right_type == INT) {
                Info* updatedInfo = tree->info;
                as_info_base(updatedInfo)->props->type = BOOL;
                update(table, tree, updatedInfo);
            } else {
                printf("Error: Operación inválida para los tipos %s, %s en línea %d.\n", type_to_str(left_type), type_to_str(right_type), as_info_base(tree->info)->props->line);
                return false;
            }
            break;
        }

        case NOT: {
            left_type = get_type_from_table(tree->left, table);
            if (left_type == BOOL) {
                Info* updatedInfo = tree->info;
                as_info_base(updatedInfo)->props->type = BOOL;
                update(table, tree, updatedInfo);
            } else {
                printf("Error: Operación inválida para el tipo %s en línea %d.\n", type_to_str(left_type), as_info_base(tree->info)->props->line);
                return false;
            }
            break;
        }

        case ASIG: {
            left_type = get_type_from_table(tree->left, table);
            right_type = get_type_from_table(tree->right, table);
            if (left_type != right_type) {
                printf("Error: Asignación inválida para los tipos %s, %s en línea %d.\n", type_to_str(left_type), type_to_str(right_type), as_info_base(tree->info)->props->line);
                return false;
            }
            break;
        }

        // case CALL_FN: {
        //     Info* info = search_global(table, as_info_base(tree->info)->props->name);
        //     LinkedList* formal = as_info_fn(info)->params;
        //     if (check_types(formal, tree->right)) {
        //         Type type_in_table = get_type_from_table(tree, table);
        //         Info* updatedInfo = tree->info;
        //         as_info_base(updatedInfo)->props->type = type_in_table;
        //     }
        // }

        default:
            break;
    }

    bool left_check = check_types(tree->left, table);
    bool right_check = check_types(tree->right, table);

    return left_check && right_check;
}

// bool check_params(LinkedList* formal, AST* current) {

// }

Type get_type_from_table(AST* node, SymbolTable* table) {
    Info* info = NULL;
    Type result;

    if (node->tag == ID) {
        info = search(table, as_info_base(node->info)->props->name);
        if (info == NULL) {
            printf("Error: Variable '%s' no declarada en linea %d.\n", as_info_base(node->info)->props->name, as_info_base(node->info)->props->line);
        }
        result = (info != NULL) ? as_info_base(info)->props->type : as_info_base(node->info)->props->type;
    }

    else if (node->tag == CALL_FN) {
        info = search_global(table, as_info_fn(node->info)->props->name);
        if (info == NULL) {
            printf("Error: Función '%s' no declarada en linea %d.\n", as_info_fn(node->info)->props->name, as_info_fn(node->info)->props->line);
        }
        if (info != NULL && as_info_fn(info)->props->is_fn == false) {
            printf("Error: '%s' no es una función en linea %d.\n", as_info_fn(node->info)->props->name, as_info_fn(node->info)->props->line);
        }
        result = (info != NULL) ? as_info_fn(info)->props->type : as_info_fn(node->info)->props->type;
    }

    // Si no lo encuentra en la tabla, usamos el tipo que tiene en el arbol
    return (info != NULL) ? result : as_info_base(node->info)->props->type;
}

int evaluate_expression(AST* expr, SymbolTable* table) {
    if (expr == NULL) {
        return -1;
    }

    Tag tag = expr->tag;

    switch (tag) {
        case VALUE:
            return as_info_base(expr->info)->props->value;

        case ID: {
            Info* info = search(table, as_info_base(expr->info)->props->name);
            if (info != NULL) {
              return as_info_base(info)->props->value;
            } else {
              return -1;
            }
        }

        case ADD: {
            return (evaluate_expression(expr->left, table) + evaluate_expression(expr->right, table));
        }

        case MUL: {
            return (evaluate_expression(expr->left, table) * evaluate_expression(expr->right, table));
        }

        case OR: {
            return (evaluate_expression(expr->left, table) || evaluate_expression(expr->right, table));
        }

        case AND: {
            return (evaluate_expression(expr->left, table) && evaluate_expression(expr->right, table));
        }

        case NOT:
            return !evaluate_expression(expr->left, table);

        default:
            return -1;
    }
}

void interpret(AST* tree, SymbolTable* table) {
    if (tree == NULL) {
        return;
    }

    if (tree->tag == ASIG) {
        Info* updatedInfo = search(table, as_info_base(tree->left->info)->props->name);
        int result = evaluate_expression(tree->right, table);
        update_value(updatedInfo, result);
        update(table, tree->left, updatedInfo);
    }

    if (tree->tag == RET) {
        int result = evaluate_expression(tree->right, table);
        Type type = get_type(as_info_base(tree->right->info)->props->name, table);
        char* to_print = value_to_str(result, type);
        printf("%s\n", to_print);
        return;
    }

    interpret(tree->left, table);
    interpret(tree->right, table);
}

Type get_type(char* name, SymbolTable* table) {
    Info* info = search(table, name);
    return as_info_base(info)->props->type;
}
