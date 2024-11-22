#pragma once

/* Tags for the AST
   Identify the type of the node.
 */
typedef enum Tag{
    ID, VALUE,
    TYPE,
    PROGRAM,
    RET, DEC, ASIG,
    EXTERN,
    SEMICOLON,
    BLOCK,
    NOT, OR, AND, ADD, MUL, SUB,
    UNMINUS, DIV, MOD,
    IF, T_E,
    WHILE,
    FN_DEC,
    FN_CALL,
    PARAM_SEC,
    LESS, GREATER, EQUAL,
    PARAMETER,
    JUMP, LABEL, IFNOT, FN_END,
    GLOBAL_DEC,
    TYPED_FC, VOID_FC,
} Tag;

char* tag_to_str(Tag tag);
