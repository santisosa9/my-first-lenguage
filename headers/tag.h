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
    NOT, OR, AND, ADD, MUL,
    IF, T_E,
    WHILE,
    CF,
    DEC_FN,
    CALL_FN,
    PARAM,
} Tag;
