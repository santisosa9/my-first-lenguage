#pragma once
#include "info.h"
#include "tag.h"
#include "utils.h"

/* Abstract Syntax Tree */
typedef struct AST
{
    Tag tag;
    Info* info;
    struct AST* left;
    struct AST* right;
} AST;

/* Create a new node using info, and set its childs. */
AST* build_root(AST* left, Info* root_info, Tag tag, AST* right);

/* Create a new node using info. */
AST* new_node(Info* info, Tag tag);

/* Join left and right trees to root. */
AST* join_trees(AST* left, AST* root, AST* right);

/* Free the tree.
   IMPORTANT: this function do NOT free the Info data.
*/
void free_tree(AST* tree);

/* Print the tree. */
void print_tree(AST* tree);

/* Print a node. */
void print_node(AST* node);

/* Performs type checking */
bool check_types(AST* tree);

/* Check if a function is well defined */
bool check_fn(AST* fn);

/* Checks if a function is well invocated */
bool check_params(LinkedList* formal_params, AST* fn_call);
