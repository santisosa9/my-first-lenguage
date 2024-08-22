#pragma once

/* Abstract Syntax Tree */
typedef struct node
{
    void* info;
    struct node* left;
    struct node* right;
} AST;


AST* new_tree(void* info, AST* left, AST* right);
AST* new_node(void* info);
void free_tree(AST* tree);
void print_tree(AST* tree, void (*print_info)(void*));
void print_info_as_int(void* info);
