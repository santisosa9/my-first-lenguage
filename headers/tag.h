#pragma once

/* Tags for the AST
   Identify the type of the node.
 */
typedef enum Tag{
    ID, VALUE,
    TYPE,
    MAIN,
    RET, DEC, ASIG,
    EXTERN,
    SEMICOLON,
    NOT, OR, AND, ADD, MUL,
    IF, THEN, ELSE,
    CF,
    FN,
} Tag;
