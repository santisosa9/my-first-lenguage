#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../headers/symbol_table.h"
#include "../headers/utils.h"

bool checkTypes (Info* left, Info* right, SymbolTable* table);

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
              printf("Declaracion exitosa de '%s' en línea %d.\n", tree->info->name, tree->info->line);
          } else {
              printf("Error: Redeclaración de la variable '%s' en línea %d.\n", tree->info->name, tree->info->line);
              return;
          }
          break;

      case ASIG:
          existing = search(table, tree->left->info->name);
          if (existing == NULL) {
              printf("Error: Asignación a variable '%s' no declarada en línea %d.\n", tree->info->name, tree->info->line);
              return;
          } else {
                Info* updatedInfo = existing->info;
                int result = evaluate_expression(tree->right, table);
                update_value(updatedInfo, result);
                update(table, tree->left, updatedInfo);
                printf("Asignación exitosa a '%s' con valor %d en línea %d.\n", existing->info->name, result, tree->info->line);
          }
          break;

      default:
          break;
    }
    
    fill_table(tree->left, table);
    fill_table(tree->right, table);
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
            if (var == NULL) {
                printf("Error: Variable '%s' no declarada en línea %d.\n", expr->info->name, expr->info->line);
                return 0;
            }
            return var->info->value;
        }

        case ADD: 
            sameType = checkTypes(expr->left->info, expr->right->info, table);
            if (sameType) {
                return evaluate_expression(expr->left, table) + evaluate_expression(expr->right, table);
            } else {
                exit(2);
            }

        case MUL: 
            return evaluate_expression(expr->left, table) * evaluate_expression(expr->right, table);

        case OR: 
            return evaluate_expression(expr->left, table) || evaluate_expression(expr->right, table);

        case AND: 
            return evaluate_expression(expr->left, table) && evaluate_expression(expr->right, table);

        case NOT: 
            return !evaluate_expression(expr->left, table);

        default:
            printf("Error: Expresión no reconocida en línea %d.\n", expr->info->line);
            return 0;
    }
}

bool checkTypes (Info* left, Info* right, SymbolTable* table) {
  if (left->type == right->type) {
    return true;
  } else {
    printf("Error: Operacion inválida para los tipos %s, %s.\n", type_to_str(left->type), type_to_str(right->type));
    return false;
  }
}