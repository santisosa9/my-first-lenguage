%{

  #include <stdlib.h>
  #include <stdio.h>

  #include "./headers/ast.h"

%}

%union {
  struct Info* INFOP; 
  struct AST* ASTP; 
  char* CHARP;
};

%token T_NRO
%token T_ID
%token T_OP_CONJUNCION
%token T_OP_DISYUNCION
%token T_TRUE
%token T_FALSE
%token T_INT
%token T_BOOL
%token T_VOID
%token T_RETURN
%token T_MAIN
    
%left '!'
%left T_OP_CONJUNCION T_OP_DISYUNCION
%left '+'  
%left '*'
 
%%
 
prog: type T_MAIN '(' ')' '{' body '}' { }
    ;

type: T_INT
    | T_BOOL
    | T_VOID
    ;

body: sentence ';'
    | sentence ';' body 
    ;

sentence: ret
        | dec
        | expr
        | asig
        ;

dec: type T_ID
   ;

expr: T_ID
    | T_NRO               
    | expr '+' expr    
    | expr '*' expr  
    | '(' expr ')'  
    | T_TRUE
    | T_FALSE
    | expr T_OP_CONJUNCION expr
    | expr T_OP_DISYUNCION expr
    | '!' expr
    ;

asig: T_ID '=' expr
    ;

ret: T_RETURN
   | T_RETURN expr
   ;

%%
