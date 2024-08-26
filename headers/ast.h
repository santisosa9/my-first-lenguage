#pragma once

typedef enum _tag{
    ID,
    MAIN,
    RET,
    STMT,
    ASSIGN,
    DEC,
    EXPR,
} Tag;

typedef enum _type
{
    INT,
    FLOAT,
    CHAR,
    STRING,
    BOOL,
    VOID
} Type;


typedef struct _info
{
    int value;
    Tag tag;
    Type type;
    char* name;
    int line;
} Info;

/* Abstract Syntax Tree */
typedef struct _node
{
    Info* info;
    struct _node* left;
    struct _node* right;
} AST;


AST* new_tree(Info* info, AST* left, AST* right);
AST* new_node(Info* info);
Info* new_info(int value, Tag tag, Type type);
void free_tree(AST* tree);
void print_tree(AST* tree);
void print_info(Info* info);
