#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../headers/symbol_table.h"
#include "../headers/utils.h"
#include "../headers/ast.h"

Info* _search_in_scope(STStackNode* scope, char* name);
void insert_params(SymbolTable* table, Info* info);
Type get_type_from_table(AST* node, SymbolTable* table);

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

bool insert(SymbolTable* table, Info* info){
    if(table == NULL) return false;

    STListNode* new_node = new_symbol_table_list_node(info, table->top->head);
    table->top->head = new_node;
    table->total_size++;
    return true;
}

void insert_params(SymbolTable* table, Info* info) {
    LinkedList* params = as_info_fn(info)->params;
    LinkedListIterator* formal_params = new_linked_list_iterator(params);
    while (has_next(formal_params)) {
        Info* param = (Info*)next(formal_params);
        insert(table, param);
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

bool decorate_tree(AST* tree, SymbolTable* table) {
    if (tree == NULL) return true;

    Tag currentTag = tree->tag;
    Info* existing;

    switch (currentTag) {
        case DEC_FN: {
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

        case CALL_FN: {
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

bool check_types_fn(AST* tree, SymbolTable* table, Type fn_return_type) {
    if (tree == NULL) {
        return true; // or false depending on your logic
    }

    Tag tag = tree->tag;
    Type right_type;

    if (tag == RET) {
        if (tree->right == NULL) {
            printf("Error: Return statement has no right child.\n");
            return false;
        }

        right_type = get_type_from_table(tree->right, table);

        if (tree->info == NULL) {
            printf("Error: tree->info is NULL.\n");
            return false; // Handle the error gracefully
        }

        if (right_type != fn_return_type) {
            printf("Error: Return invalido, para la funcion de tipo: %s, se devuelve: %s en línea %d.\n", 
                   type_to_str(fn_return_type), type_to_str(right_type), 
                   as_info_base(tree->info)->props->line);
            return false;
        }
    } else {
        if (tree->right != NULL) {
            if (!check_types_fn(tree->right, table, fn_return_type)) {
                return false;
            }
        }
        if (tree->left != NULL) {
            if (!check_types_fn(tree->left, table, fn_return_type)) {
                return false;
            }
        }
    }

    return true;
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
        
        case DEC_FN: {
            Type fn_type = get_type_from_table(tree, table);
            if (check_types_fn(tree->right,table,fn_type)){
                Info* updatedInfo = tree->info;
                as_info_base(updatedInfo)->props->type = fn_type;
            }else{
                return false;
            }
            break;
        }
        
        default:
            break;
    }

    bool left_check = check_types(tree->left, table);
    bool right_check = check_types(tree->right, table);

    return left_check && right_check;
}

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
