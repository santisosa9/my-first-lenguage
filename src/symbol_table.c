#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../headers/symbol_table.h"
#include "../headers/utils.h"
#include "../headers/ast.h"

Info* _search_in_scope(STStackNode* scope, char* name);
void insert_params(SymbolTable* table, Info* info);
void _set_offset_params(LinkedList* params);

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
                int locals = 0;
                if (!decorate_tree_fn(tree->right, table, &locals)) return false;
                InfoFunction* i_fn = as_info_fn(tree->info);
                i_fn->locals = (locals);
                _set_offset_params(i_fn->params);
                close_scope(table);
            } else {
                printf("Error: Redeclaración de la función '%s' en línea %d.\n",
                       as_info_fn(tree->info)->props->name,
                       as_info_fn(tree->info)->props->line);
                return false;
            }
            break;
        }

        case DEC: {
            existing = search_in_current_scope(table, as_info_base(tree->info)->props->name);
            if (existing == NULL) {
                if(!current_scope_is_global(table)){
                    as_info_base(tree->info)->scope = LOCAL;
                }
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
                Props* prop = new_prop(NO_TYPED, 0, "", 0, 0,get_file_path());
                copy_prop(prop, as_info_fn(existing)->props);
                prop->line = as_info_fn(tree->info)->props->line;
                Info* i_fn = new_info_fn(prop, as_info_fn(existing)->params, as_info_fn(existing)->is_extern);
                as_info_fn(i_fn)->cant_params = as_info_fn(existing)->cant_params;  
                tree->info = i_fn;
            }
            if (!decorate_tree(tree->right, table)) return false;
            break;
        }

        default: {
            if (!decorate_tree(tree->left, table) || !decorate_tree(tree->right, table)) return false;
            break;
        }
    }

    return true;
}

bool decorate_tree_fn(AST* tree, SymbolTable* table, int* locals) {
    if (tree == NULL) return true;

    Tag currentTag = tree->tag;
    Info* existing;

    switch (currentTag) {
        case BLOCK: {
            open_scope(table);
            if (!decorate_tree_fn(tree->right, table, locals)) return false;
            close_scope(table);
            break;
        }

        case DEC: {
            existing = search_in_current_scope(table, as_info_base(tree->info)->props->name);
            if (existing == NULL) {
                // actualizo el scope y el offset
                as_info_base(tree->info)->scope = LOCAL;
                (*locals)++;
                as_info_base(tree->info)->props->offset = (-4 * (*locals)) -4;
                insert(table, tree->info);
            } else {
                printf("Error: Redeclaración de la variable '%s' en línea %d.\n",
                    as_info_base(tree->info)->props->name,
                    as_info_base(tree->info)->props->line);
                return false;
            }
            if (!decorate_tree_fn(tree->left, table, locals) || !decorate_tree_fn(tree->right, table, locals)) return false;
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
                Props* prop = new_prop(NO_TYPED, 0, "", 0, 0,get_file_path());
                copy_prop(prop, as_info_fn(existing)->props);
                prop->line = as_info_fn(tree->info)->props->line;
                Info* i_fn = new_info_fn(prop, as_info_fn(existing)->params, as_info_fn(existing)->is_extern);
                as_info_fn(i_fn)->cant_params = as_info_fn(existing)->cant_params;  
                tree->info = i_fn;
            }
            (*locals)++;
            as_info_base(tree->info)->props->offset = (-4 * (*locals)) -4;
            if (!decorate_tree_fn(tree->right, table, locals)) return false;
            break;
        }

        case ADD: 
        case SUB: 
        case MUL: 
        case DIV: 
        case LESS:
        case GREATER:
        case EQUAL:
        case AND:
        case OR: {
            (*locals)++;
            as_info_base(tree->info)->props->offset = (-4 * (*locals)) -4;

            if (!decorate_tree_fn(tree->left, table, locals)) return false;
            if (!decorate_tree_fn(tree->right, table, locals)) return false;
            break;
        }

        default: {
            if (!decorate_tree_fn(tree->left, table, locals) || !decorate_tree_fn(tree->right, table, locals)) return false;
            break;
        }
    }

    return true;
}

void _set_offset_params(LinkedList* params) {
    LinkedListIterator* iterator = new_linked_list_iterator(params);
    int offset = 4;
    while (has_next(iterator)) {
        Info* data = (Info*)next(iterator);
        as_info_base(data)->props->offset = offset;
        offset += 4;
    }
    free(iterator);
}