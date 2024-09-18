#pragma once

#include <stdbool.h>
#include "ast.h"

typedef struct SymbolTableNode
{
    Info* info;
    struct SymbolTableNode* next;
} SymbolTableNode;

typedef struct SymbolTable
{
    int size;
    SymbolTableNode* head;
} SymbolTable;

/* Returns a new SymbolTableNode initialized with info. */
SymbolTableNode* new_symbol_table_node(Info* info);

/* Returns a new emprty SymbolTable. */
SymbolTable* new_symbol_table();

/* Insert new info in the table. */
bool insert(SymbolTable* table, Info* info);

/* Erase the info with the given name from the table and retrun it. */
SymbolTableNode* erase(SymbolTable* table, char* name);

/* Search for the info with the given name in the table.
   Returns a pointer to the node if was found, or NULL if not.
*/
SymbolTableNode* search(SymbolTable* table, char* name);

/* Update the fields of an info inside the table searched by it name. */
// bool update(SymbolTable* table, Info* info);

bool update(SymbolTable* table, AST* tree, Info* info);

/* Print the table. */
void print_table(SymbolTable* table);

/* Free the table.
   NOTE: Free the Info data.
*/
void free_table(SymbolTable* table);

void fill_table(AST* tree, SymbolTable* table);

int evaluate_expression(AST* expr, SymbolTable* table);

bool checkTypes (AST* left, AST* right, SymbolTable* table);

void interpret(AST* tree, SymbolTable* table);