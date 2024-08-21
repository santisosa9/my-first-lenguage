%{

#include <stdlib.h>
#include <stdio.h>

%}
 
%token NRO
%token ID
%token OP_CONJUNCION
%token OP_DISYUNCION
%token TRUE_CTE
%token FALSE_CTE
%token INT
%token BOOL
%token VOID
%token RETURN
%token MAIN
    
%left '!'
%left OP_CONJUNCION OP_DISYUNCION
%left '+'  
%left '*'
 
%%
 
prog: type MAIN '(' ')' '{' body '}'
    ;

type: INT
    | BOOL
    | VOID
    ;

body: sentence ';'
    | sentence ';' body 
    ;

sentence: ret
        | dec
        | expr
        | asig
        ;

dec: type ID
   ;

expr: ID
    | NRO               
    | expr '+' expr    
    | expr '*' expr  
    | '(' expr ')'  
    | TRUE_CTE
    | FALSE_CTE
    | expr OP_CONJUNCION expr
    | expr OP_DISYUNCION expr
    | '!' expr
    ;

asig: ID '=' expr
    ;

ret: RETURN
   | RETURN expr
   ;

%%
