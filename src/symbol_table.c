#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../headers/symbol_table.h"
#include "../headers/utils.h"

Type get_type_from_table(AST* node, SymbolTable* table);
Type get_type(char* name, SymbolTable* table);

SymbolTable* new_symbol_table(){
    SymbolTable* table = (SymbolTable*)malloc(sizeof(SymbolTable));
    table->size = 0;
    table->head = NULL;
    return table;
}

SymbolTableNode* new_symbol_table_node(Info* info){
    SymbolTableNode* node = (SymbolTableNode*)malloc(sizeof(SymbolTableNode));
    node->info = info;
    node->next = NULL;
    return node;
}

bool insert(SymbolTable* table, Info* info){
    if(table == NULL) return false;

    SymbolTableNode* new_node = new_symbol_table_node(info);
    SymbolTableNode* current = table->head;

    while (table->size > 0 && current->next != NULL) {
        if(strcmp(current->info->name, info->name) == 0){
            return false;
        }
        current = current->next;
    }

    if(table->size == 0){
        table->head = new_node;
    } else {
        current->next = new_node;
    }

    table->size++;
    return true;
}

SymbolTableNode* search(SymbolTable* table, char* name){
    if(table == NULL) return NULL;

    SymbolTableNode* current = table->head;

    while (current != NULL) {
        if(strcmp(current->info->name, name) == 0){
            return current;
        }
        current = current->next;
    }

    return NULL;
}

bool update(SymbolTable* table, AST* tree, Info* info){
    if(table == NULL) return false;
    if (tree == NULL) return false;

    SymbolTableNode* target = search(table, info->name);

    if(target == NULL) return false;

    copy_info(target->info, info);
    tree->info = target->info;

    return true;
}


SymbolTableNode* erase(SymbolTable* table, char* name){
    if(table == NULL) return false;

    SymbolTableNode* current = table->head;
    SymbolTableNode* prev = NULL;

    while (current != NULL) {
        if(strcmp(current->info->name, name) == 0){
            if(prev == NULL){
                table->head = current->next;
            } else {
                prev->next = current->next;
            }
            table->size--;
            return current;
        }
        prev = current;
        current = current->next;
    }

    return NULL;
}

void print_table(SymbolTable* table){
    if(table == NULL) return;

    SymbolTableNode* current = table->head;

    printf("Table: \n");
    while (current != NULL) {
        print_info(current->info);
        current = current->next;
    }
}

void free_table(SymbolTable* table){
    if(table == NULL) return;

    SymbolTableNode* current = table->head;
    SymbolTableNode* next = NULL;

    while (current != NULL) {
        next = current->next;
        free(current->info);
        free(current);
        current = next;
    }

    free(table);
}

void fill_table(AST* tree, SymbolTable* table) {
    if (tree == NULL) {
        return;
    }

    SymbolTableNode* existing;
    Tag currentTag = tree->tag;

    switch (currentTag) {
      case DEC:
            existing = search(table, tree->info->name);
            if (existing == NULL) {
                insert(table, tree->info);
            } else {
                printf("Error: Redeclaración de la variable '%s' en línea %d.\n", tree->info->name, tree->info->line);
                return;
            }
            break;

      default:
          break;
    }

    fill_table(tree->left, table);
    fill_table(tree->right, table);
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
                updatedInfo->type = INT;
                update(table, tree, updatedInfo);
            } else {
                printf("Error: Operación inválida para los tipos %s, %s en línea %d.\n", type_to_str(left_type), type_to_str(right_type), tree->info->line);
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
                updatedInfo->type = BOOL;
                update(table, tree, updatedInfo);
            } else {
                printf("Error: Operación inválida para los tipos %s, %s en línea %d.\n", type_to_str(left_type), type_to_str(right_type), tree->info->line);
                return false;
            }
            break;
        }

        case NOT: {
            left_type = get_type_from_table(tree->left, table);
            if (left_type == BOOL) {
                Info* updatedInfo = tree->info;
                updatedInfo->type = BOOL;
                update(table, tree, updatedInfo);
            } else {
                printf("Error: Operación inválida para el tipo %s en línea %d.\n", type_to_str(left_type), tree->info->line);
                return false;
            }
            break;
        }

        case ASIG: {
            left_type = get_type_from_table(tree->left, table);
            right_type = get_type_from_table(tree->right, table);
            if (left_type != right_type) {
                printf("Error: Asignación inválida para los tipos %s, %s en línea %d.\n", type_to_str(left_type), type_to_str(right_type), tree->info->line);
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
    SymbolTableNode* node_in_table = NULL;

    if (node->tag == ID) {
        node_in_table = search(table, node->info->name);
        if (node_in_table == NULL) {
            printf("Error: Variable '%s' no declarada en linea %d.\n", node->info->name, node->info->line);
        }
    }

    // Si no lo encuentra en la tabla, usamos el tipo que tiene en el arbol
    return (node_in_table != NULL) ? node_in_table->info->type : node->info->type;
}

int evaluate_expression(AST* expr, SymbolTable* table) {
    if (expr == NULL) {
        return -1;
    }

    Tag tag = expr->tag;
    bool sameType;

    switch (tag) {
        case VALUE:
            return expr->info->value;

        case ID: {
            SymbolTableNode* var = search(table, expr->info->name);
            if (var != NULL) {
              return var->info->value;
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
        SymbolTableNode* existing = search(table, tree->left->info->name);
        Info* updatedInfo = existing->info;
        int result = evaluate_expression(tree->right, table);
        update_value(updatedInfo, result);
        update(table, tree->left, updatedInfo);
    }

    if (tree->tag == RET) {
        int result = evaluate_expression(tree->right, table);
        Type type = get_type(tree->right->info->name, table);
        char* to_print = value_to_str(result, type);
        printf("%s\n", to_print);
        return; 
    }

    interpret(tree->left, table);
    interpret(tree->right, table);
}

Type get_type(char* name, SymbolTable* table) {
    SymbolTableNode* e = search(table, name);
    return e->info->type;
}