#pragma once

typedef enum Tag{
    ID,
    MAIN,
    RET,
    STMT,
    ASIG,
    TYPE,
    COMMA,
    DEC,
    NOT,
    OR,
    AND,
    ADD,
    MUL,
    VALUE,
} Tag;

typedef enum Type
{
    INT,
    FLOAT,
    CHAR,
    STRING,
    BOOL,
    VOID,
    NO_TYPED,
    ANY
} Type;


typedef struct Info
{
    Tag tag;
    Type type;
    int value;
    char* name;
    int line;
    int col;
} Info;

/* Abstract Syntax Tree */
typedef struct AST
{
    Info* info;
    struct AST* left;
    struct AST* right;
} AST;

/* Create a new node using info, and set its childs */
AST* build_root(AST* left, Info* root_info, AST* right);

/* Create a new node using info */
AST* new_node(Info* info);

/* Join left and right trees to root */
AST* join_trees(AST* left, AST* root, AST* right);

/* Create a new info */
Info* new_info(Tag tag, Type type, int value, char* name, int line, int col);

/* Free the tree */
void free_tree(AST* tree);

/* Print the tree */
void print_tree(AST* tree);

/* Print the info */
void print_info(Info* info);

char* tag_to_str(Tag tag);

char* type_to_str(Type type);

char* value_to_str(int value, Type type);
