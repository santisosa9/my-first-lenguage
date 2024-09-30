%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>

    #include "../headers/symbol_table.h"
    #include "../headers/ast.h"
    #include "../headers/utils.h"

    AST* global_tree = NULL;
    AST* get_parsed_tree();

    void yyerror(const char* s);
    int yylex();

%}

%union {
  union Info* INFOP;
  struct AST* ASTP;
  char* CHARP;
  Type TYPE;
};

%token <INFOP> T_ASIG
%token <INFOP> T_SEMICOLON
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
%token <INFOP> T_PROGRAM
%token <INFOP> T_IF
%token <INFOP> T_THEN
%token <INFOP> T_ELSE
%token <INFOP> T_WHILE
%token <INFOP> T_EXTERN

%left T_NOT
%left T_AND T_OR
%left T_ADD
%left T_MUL

%type <ASTP> prog
%type <TYPE> type_fn
%type <TYPE> type_var
%type <ASTP> body
%type <ASTP> block
%type <ASTP> statement
%type <ASTP> dec
%type <ASTP> dec_fn
%type <ASTP> expr
%type <ASTP> exprs
%type <ASTP> asig
%type <ASTP> ret
%type <ASTP> while
%type <ASTP> if

%%

prog: T_PROGRAM block                    { global_tree = build_root(NULL, $1, PROGRAM, $2); }
    ;

type_fn: type_var                        { $$ = $1; }
       | T_VOID                          { $$ = VOID; }
       ;

type_var: T_INT                          { $$ = INT; }
        | T_BOOL                         { $$ = BOOL; }
        ;

block: '{' body '}'                      { $$ = $2; }
     ;

body: statement                          { $$ = $1; }
    | statement body                     { $$ = build_root($1, NULL, SEMICOLON, $2); }
    ;

statement: ret T_SEMICOLON               { $$ = $1; }
         | dec T_SEMICOLON               { $$ = $1; }
         | asig T_SEMICOLON              { $$ = $1; }
         | if                            { $$ = $1; }
         | while                         { $$ = $1; }
         | block                         { $$ = $1; }
         | dec_fn                        { $$ = $1; } 
         ;

dec_fn: type_fn T_ID '(' param ')' block      {printf("dec fn block"); }
      | type_fn T_ID '(' param ')' T_EXTERN   {printf("dec fn extern"); }
      ;

call_fn: T_ID '(' exprs ')'                    {printf("call fn"); }
       ;

param: type_var T_ID                           {printf("param"); }
     | type_var T_ID ',' param                 {printf("params"); }
     ;

dec: type_var T_ID                      { AST* node = new_node($2, DEC);
                                          as_info_base(node->info)->props->type = $1;
                                          $$ = node;
                                        }
   ;

exprs: expr                             { printf("expr"); }
     | expr ',' exprs                   { printf("exprs"); } 
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
    | call_fn                           { printf("call fn"); }
    ;

asig: T_ID T_ASIG expr                  { $$ = join_trees(new_node($1, ID), new_node($2, ASIG), $3); }
    ;

ret: T_RET                              { $$ = new_node($1, RET); }
   | T_RET expr                         { $$ = build_root(NULL, $1, RET, $2); }
   ;

if: T_IF '(' expr ')' T_THEN block                       { $$ = build_root($3, $1, IF, $6); }
  | T_IF '(' expr ')' T_THEN block T_ELSE block          { AST* t_e = build_root($6, NULL, T_E, $8);
                                                           $$ = build_root($3, $1, IF, t_e); 
                                                         }
  ;

while: T_WHILE '(' expr ')' block       { $$ = build_root($3, $1, WHILE, $5); }
  ;
%%


AST* get_parsed_tree() {
    return global_tree;
}
