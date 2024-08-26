#pragma once




typedef int Tag;

typedef enum _type
{
    INT,
    FLOAT,
    CHAR,
    STRING,
    BOOL,
    VOID
} Type;


typedef struct info
{
    int value;
    Tag tag;
    Type type;
} Info;

/* Abstract Syntax Tree */
typedef struct node
{
    Info* info;
    struct node* left;
    struct node* right;
} AST;


AST* new_tree(Info* info, AST* left, AST* right);
AST* new_node(Info* info);
Info* new_info(int value, Tag tag, Type type);
void free_tree(AST* tree);
void print_tree(AST* tree);
void print_info(Info* info);
