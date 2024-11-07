%{
    #include <stdlib.h>
    #include <string.h>
    #include <stdio.h>

    #include "../headers/ast.h"
    #include "../headers/utils.h"
    #include "../headers/parser.h"

    void yyerror();
%}

/* Opciones */

/* Esto indica que solo lea un archivo de entrada */
%option noyywrap
/* Permite obtener el numero de linea */
%option yylineno


/* Tokens */
DIGITO          [0-9]
LETRA           [a-zA-Z]
ID              {LETRA}({LETRA}|{DIGITO}|_)*

NRO         	(-?[1-9][0-9]{0,4})|0


/* Reglas */

%%

  /* Info* new_info(Tag tag, Type type, int value, char* name, int line, int col); */

";" 		      { yylval.INFOP = new_info(NO_TYPED, atoi(yytext), strdup(yytext), yylineno, 0, get_file_path());
                return T_SEMICOLON; }

"extern"      { yylval.INFOP = new_info(NO_TYPED, atoi(yytext), strdup(yytext), yylineno, 0, get_file_path());
                return T_EXTERN; }

"if"          { yylval.INFOP = new_info(NO_TYPED, atoi(yytext), strdup(yytext), yylineno, 0, get_file_path());
                return T_IF; }

"then"        { yylval.INFOP = new_info(NO_TYPED, atoi(yytext), strdup(yytext), yylineno, 0, get_file_path());
                return T_THEN; }

"else"        { yylval.INFOP =new_info(NO_TYPED, atoi(yytext), strdup(yytext), yylineno, 0, get_file_path());
                return T_ELSE; }

"while"       { yylval.INFOP = new_info(NO_TYPED, atoi(yytext), strdup(yytext), yylineno, 0, get_file_path());
                return T_WHILE; }

"="			      { yylval.INFOP = new_info(NO_TYPED, atoi(yytext), strdup(yytext), yylineno, 0, get_file_path());
                return T_ASIG; }

"+"		  	    { yylval.INFOP = new_info(INT, atoi(yytext), strdup(yytext), yylineno, 0, get_file_path());
                return T_ADD; }

"-"		  	    { yylval.INFOP = new_info(INT, atoi(yytext), strdup(yytext), yylineno, 0, get_file_path());
                return T_SUB; }

"/"		  	    { yylval.INFOP = new_info(INT, atoi(yytext), strdup(yytext), yylineno, 0, get_file_path());
                return T_DIV; }

"%"		  	    { yylval.INFOP = new_info(INT, atoi(yytext), strdup(yytext), yylineno, 0, get_file_path());
                return T_MOD; }

"*"		  	    { yylval.INFOP = new_info(INT, atoi(yytext), strdup(yytext), yylineno, 0, get_file_path());
                return T_MUL; }

">"           { yylval.INFOP = new_info(BOOL, atoi(yytext), strdup(yytext), yylineno, 0, get_file_path());
                return T_GREATER; }

"<"           { yylval.INFOP = new_info(BOOL, atoi(yytext), strdup(yytext), yylineno, 0, get_file_path());
                return T_LESS; }

"=="          { yylval.INFOP = new_info(BOOL, atoi(yytext), strdup(yytext), yylineno, 0, get_file_path());
                return T_EQUAL; }

"!"		  	    { yylval.INFOP = new_info(BOOL, atoi(yytext), strdup(yytext), yylineno, 0, get_file_path());
                return T_NOT; }

"||"		      { yylval.INFOP = new_info(BOOL, atoi(yytext), strdup(yytext), yylineno, 0, get_file_path());
                return T_OR; }

"&&"		      { yylval.INFOP = new_info(BOOL, atoi(yytext), strdup(yytext), yylineno, 0, get_file_path());
                return T_AND; }

"true"		    { yylval.INFOP = new_info(BOOL, atoi(yytext), strdup(yytext), yylineno, 0, get_file_path());
                return T_TRUE; }

"false"		    { yylval.INFOP = new_info(BOOL, atoi(yytext), strdup(yytext), yylineno, 0, get_file_path());
                return T_FALSE; }

"int"		      { yylval.INFOP = new_info(INT, atoi(yytext), strdup(yytext), yylineno, 0, get_file_path());
                return T_INT; }

"integer"	    { yylval.INFOP = new_info(INT, atoi(yytext), strdup(yytext), yylineno, 0, get_file_path());
                return T_INT; }

"bool"		    { yylval.INFOP = new_info(BOOL, atoi(yytext), strdup(yytext), yylineno, 0, get_file_path());
                return T_BOOL; }

"void"		    { yylval.INFOP = new_info(VOID, atoi(yytext), strdup(yytext), yylineno, 0, get_file_path());
                return T_VOID; }

"return"	    { yylval.INFOP = new_info(NO_TYPED, atoi(yytext), strdup(yytext), yylineno, 0, get_file_path());
                return T_RET; }

"Program"	    { yylval.INFOP = new_info(NO_TYPED, atoi(yytext), strdup(yytext), yylineno, 0, get_file_path());
                return T_PROGRAM; }

{ID}		      { yylval.INFOP = new_info(ANY, atoi(yytext), strdup(yytext), yylineno, 0, get_file_path());
                return T_ID; }

{NRO}		      { yylval.INFOP = new_info(INT, atoi(yytext), strdup(yytext), yylineno, 0, get_file_path());
                return T_NRO; }

"//".*        { /* DO NOTHING */ }

[/][*][^*]*[*]+([^*/][^*]*[*]+)*[/]  { /* DO NOTHING */ }

[{}(),]        { return *yytext; }

[ \t\n]       ;

.			  { printf("ERROR: %s\n",yytext);
                yyerror(); }

%%

void yyerror(){
    printf("-> SYNTAX ERROR at: %d\n", yylineno);
    int line_len = strlen(itos(yylineno));
    printf("  %d  | %s\n", yylineno, yytext);
    printf("%s|\n", repeat_char(' ', line_len + 4));
}
