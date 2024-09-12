%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>

    #include "headers/symbol_table.h"
    #include "headers/ast.h"
    #include "headers/utils.h"
    AST* global_tree = NULL;
    SymbolTable* global_table = NULL;
%}

%union {
  struct Info* INFOP;
  struct AST* ASTP;
  char* CHARP;
  Type TYPE;
};

%token <INFOP> T_ASIG
%token <INFOP> T_COMMA
%token <INFOP> T_NRO
%token <INFOP> T_ID
%token <INFOP> T_ADD
%token <INFOP> T_MUL
%token <INFOP> T_AND
%token <INFOP> T_OR
%token <INFOP> T_NOT
%token <INFOP> T_TRUE
%token <INFOP> T_FALSE
%token <INFOP> T_INT
%token <INFOP> T_BOOL
%token <INFOP> T_VOID
%token <INFOP> T_RET
%token <INFOP> T_MAIN

%left T_NOT
%left T_AND T_OR
%left T_ADD
%left T_MUL

%type <ASTP> prog
%type <TYPE> type
%type <ASTP> body
%type <ASTP> sentence
%type <ASTP> dec
%type <ASTP> expr
%type <ASTP> asig
%type <ASTP> ret

%%

prog: type T_MAIN '(' ')' '{' body '}'  { Info* i = $2; 
                                          i->type = $1; 
                                          AST* tree = build_root(NULL, $2, MAIN, $6); 
                                          print_tree(tree); 
                                          global_tree = tree; 
                                          global_table = new_symbol_table(); 
                                          fill_table(global_tree, global_table); 
                                          print_table(global_table); 
                                          print_tree(tree);
                                        }
    ;

type: T_INT                             { $$ = INT; }
    | T_BOOL                            { $$ = BOOL; }
    | T_VOID                            { $$ = VOID; }
    ;

body: sentence T_COMMA                  { $$ = $1; }
    | sentence T_COMMA body             { $$ = build_root($1, $2, COMMA, $3); }
    ;

sentence: ret                           { $$ = $1; }
        | dec                           { $$ = $1; }
        | expr                          { $$ = $1; }
        | asig                          { $$ = $1; }
        ;

dec: type T_ID                          { AST* node = new_node($2, DEC); node->info->type = $1; $$ = node; }
   ;

expr: expr T_ADD expr                   { $$ = build_root($1, $2, ADD, $3); }
    | expr T_MUL expr                   { $$ = build_root($1, $2, MUL, $3); }
    | expr T_AND expr                   { $$ = build_root($1, $2, AND, $3); }
    | expr T_OR  expr                   { $$ = build_root($1, $2, OR, $3); }
    | T_NOT expr                        { $$ = build_root(NULL, $1, NOT, $2); }
    | '(' expr ')'                      { $$ = $2; }
    | T_ID                              { $$ = new_node($1, ID); }
    | T_NRO                             { $$ = new_node($1, VALUE); }
    | T_TRUE                            { $$ = new_node($1, VALUE); }
    | T_FALSE                           { $$ = new_node($1, VALUE); }
    ;

asig: T_ID T_ASIG expr                  { $$ = join_trees(new_node($1, ID), new_node($2, ASIG), $3); }
    ;

ret: T_RET                              { $$ = new_node($1, RET); }
   | T_RET expr                         { $$ = build_root(NULL, $1, RET, $2); }
   ;

%%


AST* get_parsed_tree() {
    return global_tree;
}
