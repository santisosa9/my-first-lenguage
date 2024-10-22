#pragma once

#include <stdbool.h>
#include "ast.h"

typedef struct STListNode
{
    Info* info;
    struct STListNode* next;
} STListNode;

typedef struct STStackNode
{
    STListNode* head;
    struct STStackNode* next;
    struct STStackNode* prev;
} STStackNode;

typedef struct SymbolTable
{
    nat scope_size;
    nat total_size;
    STStackNode* base;
    STStackNode* top;
} SymbolTable;

/* Returns a new STListNode initialized with info. */
STListNode* new_symbol_table_list_node(Info* info, STListNode* next);

/* Returns a new STStackNode. */
STStackNode* new_symbol_table_stack_node(STListNode* head, STStackNode* next, STStackNode* prev);

/* Returns a new emprty SymbolTable. */
SymbolTable* new_symbol_table();

/* Insert new info in the table at the top (last opened scope). */
bool insert(SymbolTable* table, Info* info);

/* Erase the info with the given name from the table and retrun it. */
Info* erase(SymbolTable* table, char* name);

/* Search for the info with the given name in the table.
   Returns a pointer to the node if was found, or NULL if not.
*/
Info* search(SymbolTable* table, char* name);

/* Search for the function with the given name in the table.
   Returns a pointer to the node if was found, or NULL if not.
*/
Info* search_global(SymbolTable* table, char* name);

/* Open a new scope in the table. */
void open_scope(SymbolTable* table);

/* Close the current scope in the table. */
void close_scope(SymbolTable* table);

/* Update the fields of an info inside the table searched by it name. */
// bool update(SymbolTable* table, Info* info);

bool update(SymbolTable* table, AST* tree, Info* info);

/* Print the table. */
void print_table(SymbolTable* table);

/*
   IMPORTANT: Next free functions DO NOT FREE the Info data, just the structures
   that hold it.
*/

/* Free the table. */
void free_table(SymbolTable* table);

/* Free the STListNode. */
void free_symbol_table_list_node(STListNode* node);

/* Free the STStackNode. */
void free_symbol_table_stack_node(STStackNode* node);

/* Fill the SymbolTable with data in the AST */
bool decorate_tree(AST* tree, SymbolTable* table);

bool check_types(AST* tree);

bool check_fn(AST* fn);

bool check_params(LinkedList* formal_params, AST* fn_call);

