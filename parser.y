%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>

    #include "../headers/ast.h"
    #include "../headers/linked_list.h"
    #include "../headers/symbol_table.h"
    #include "../headers/utils.h"

    AST* global_tree = NULL;
    AST* get_parsed_tree();

    void yyerror(const char* s);
    int yylex();
%}

%union {
    union Info* INFOP;
    struct AST* ASTP;
    struct LinkedList* LISTP;
    char* CHARP;
    Type TYPE;
};

%token <INFOP> T_ASIG
%token <INFOP> T_SEMICOLON
%token <INFOP> T_NRO
%token <INFOP> T_ID
%token <INFOP> T_ADD
%token <INFOP> T_MUL
%token <INFOP> T_LESS
%token <INFOP> T_GREATER
%token <INFOP> T_EQUAL
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
%token <INFOP> T_UNSUB
%token <INFOP> T_SUB
%token <INFOP> T_DIV
%token <INFOP> T_MOD

%left T_AND T_OR
%nonassoc T_EQUAL T_LESS T_GREATER
%left T_ADD T_SUB T_UNSUB
%left T_MUL T_DIV T_MOD
%left T_UNMINUS

%type <ASTP> prog
%type <TYPE> type_var
%type <ASTP> body
%type <ASTP> body_fn
%type <ASTP> block
%type <ASTP> statement
%type <ASTP> dec
%type <ASTP> dec_fn
%type <ASTP> call_fn
%type <ASTP> expr
%type <ASTP> param_exprs
%type <ASTP> asig
%type <ASTP> ret
%type <ASTP> while
%type <ASTP> if
%type <LISTP> param

%%

prog: T_PROGRAM '{' body '}' {
        global_tree = build_root(NULL, NULL, PROGRAM, $3);
    };

type_var: T_INT {
        $$ = INT;
    }
    | T_BOOL {
        $$ = BOOL;
    }
    | T_VOID {
        $$ = VOID;
    };

body: dec {
        $$ = $1;
    }
    | dec body {
        $$ = build_root($1, NULL, SEMICOLON, $2);
    }
    | dec_fn {
        $$ = $1;
    }
    | dec_fn body {
        $$ = build_root($1, NULL, SEMICOLON, $2);
    };

block: '{' body_fn '}' {
        Info* info = new_info(NO_TYPED, 0, "block", 0, 0);
        $$ = build_root(NULL, info, BLOCK, $2);
    };

body_fn: dec {
        $$ = $1;
    }
    | dec body_fn {
        $$ = build_root($1, NULL, SEMICOLON, $2);
    }
    | statement {
        $$ = $1;
    }
    | statement body_fn {
        $$ = build_root($1, NULL, SEMICOLON, $2);
    };

statement: asig T_SEMICOLON {
        $$ = $1;
    }
    | call_fn T_SEMICOLON {
        $$ = $1;
    }
    | if {
        $$ = $1;
    }
    | while {
        $$ = $1;
    }
    | ret T_SEMICOLON {
        $$ = $1;
    }
    | block {
        $$ = $1;
    };

dec_fn: type_var T_ID '(' param ')' '{' body_fn '}' {
        Info* i_fn = new_info_fn(as_info_base($2)->props, $4, false);
        as_info_fn(i_fn)->props->type = $1;
        $$ = build_root(NULL, i_fn, FN_DEC, $7);
    }
    | type_var T_ID '(' ')' '{' body_fn '}' {
        Info* i_fn = new_info_fn(as_info_base($2)->props, new_linked_list(), false);
        as_info_fn(i_fn)->props->type = $1;
        $$ = build_root(NULL, i_fn, FN_DEC, $6);
    }
    | type_var T_ID '(' param ')' T_EXTERN T_SEMICOLON {
        Info* i_fn = new_info_fn(as_info_base($2)->props, $4, true);
        as_info_fn(i_fn)->props->type = $1;
        $$ = build_root(NULL, i_fn, FN_DEC, NULL);
    }
    | type_var T_ID '(' ')' T_EXTERN T_SEMICOLON {
        Info* i_fn = new_info_fn(as_info_base($2)->props, new_linked_list(), true);
        as_info_fn(i_fn)->props->type = $1;
        $$ = build_root(NULL, i_fn, FN_DEC, NULL);
    };

param: type_var T_ID {
        InfoBase* i_base = as_info_base($2);
        i_base->props->type = $1;
        i_base->scope = PARAM;
        LinkedList* params = new_linked_list();
        insert_head(params, i_base);
        $$ = params;
    }
    | type_var T_ID ',' param {
        InfoBase* i_base = as_info_base($2);
        i_base->props->type = $1;
        i_base->scope = PARAM;
        LinkedList* params = $4;
        insert_head(params, i_base);
        $$ = params;
    };

call_fn: T_ID '(' param_exprs ')' {
        LinkedList* params = new_linked_list();
        Info* i_fn = new_info_fn(as_info_base($1)->props, params, false);
        as_info_fn(i_fn)->props->type = NO_TYPED;
        $$ = join_trees(NULL, new_node(i_fn, FN_CALL), $3);
    }
    | T_ID '(' ')' {
        LinkedList* params = new_linked_list();
        Info* i_fn = new_info_fn(as_info_base($1)->props, params, false);
        as_info_fn(i_fn)->props->type = NO_TYPED;
        $$ = join_trees(NULL, new_node(i_fn, FN_CALL), NULL);
    };

param_exprs: expr {
        $$ = build_root($1, NULL, PARAM_SEC, NULL);
    }
    | expr ',' param_exprs {
        $$ = build_root($1, NULL, PARAM_SEC, $3);
    };

dec: type_var T_ID T_SEMICOLON {
        AST* node = new_node($2, DEC);
        as_info_base(node->info)->props->type = $1;
        $$ = node;
    }
    | type_var T_ID T_ASIG expr T_SEMICOLON {
        as_info_base($2)->props->type = $1;
        $$ = join_trees(new_node($2, ID), new_node($2, DEC), $4);
    };

expr: expr T_ADD expr {
        $$ = build_root($1, $2, ADD, $3);
    }
    | expr T_MUL expr {
        $$ = build_root($1, $2, MUL, $3);
    }
    | expr T_AND expr {
        $$ = build_root($1, $2, AND, $3);
    }
    | expr T_DIV expr {
        $$ = build_root($1, $2, DIV, $3);
    }
    | expr T_SUB expr {
        $$ = build_root($1, $2, SUB, $3);
    }
    | expr T_MOD expr {
        $$ = build_root($1, $2, MOD, $3);
    }
    | expr T_OR expr {
        $$ = build_root($1, $2, OR, $3);
    }
    | expr T_EQUAL expr {
        $$ = build_root($1, $2, EQUAL, $3);
    }
    | expr T_LESS expr {
        $$ = build_root($1, $2, LESS, $3);
    }
    | expr T_GREATER expr {
        $$ = build_root($1, $2, GREATER, $3);
    }
    | T_NOT expr %prec T_UNMINUS {
        $$ = build_root(NULL, $1, NOT, $2);
    }
    | '(' expr ')' {
        $$ = $2;
    }
    | T_ID {
        $$ = new_node($1, ID);
    }
    | T_NRO {
        $$ = new_node($1, VALUE);
    }
    | T_TRUE {
        $$ = new_node($1, VALUE);
    }
    | T_FALSE {
        $$ = new_node($1, VALUE);
    }
    | call_fn {
        $$ = $1;
    };

asig: T_ID T_ASIG expr {
        $$ = join_trees(new_node($1, ID), new_node($2, ASIG), $3);
    };

ret: T_RET {
        $$ = new_node($1, RET);
    }
    | T_RET expr {
        $$ = build_root(NULL, $1, RET, $2);
    };

if: T_IF '(' expr ')' T_THEN block {
        $$ = build_root($3, $1, IF, $6);
    }
    | T_IF '(' expr ')' T_THEN block T_ELSE block {
        AST* t_e = build_root($6, NULL, T_E, $8);
        $$ = build_root($3, $1, IF, t_e);
    };

while: T_WHILE '(' expr ')' block {
        $$ = build_root($3, $1, WHILE, $5);
    };

%%

AST* get_parsed_tree() {
    return global_tree;
}
