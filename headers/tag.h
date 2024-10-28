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
    UNSUB, DIV, MOD,
    IF, T_E,
    WHILE,
    FN_DEC,
    FN_CALL,
    PARAM_SEC,
    LESS, GREATER, EQUAL,
    FN, 
    JUMP, LABEL, IFF,
} Tag;

char* tag_to_str(Tag tag);
