%{
	#include <stdlib.h>
	#include <string.h>
	#include <stdio.h>

	#include "headers/ast.h"
	#include "headers/utils.h"
	#include "preproyecto-sintaxis.tab.h"

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
ID              {LETRA}({LETRA}|{DIGITO})*

NRO         	(-?[1-9][0-9]{0,4})|0


/* Reglas */

%%

 /* Info* new_info(Tag tag, Type type, int value, char* name, int line, int col); */

";" 		  { /* printf("%s\n",yytext); */
				yylval.INFOP = new_info(COMMA, NO_TYPED, 0, strdup(yytext), yylineno, 0);
				return T_COMMA; }

"="			  { /* printf("%s\n",yytext); */
				yylval.INFOP = new_info(ASIG, NO_TYPED, 0, strdup(yytext), yylineno, 0);
				return T_ASIG; }

"+"		  	  { /* printf("%s\n",yytext); */
				yylval.INFOP = new_info(ADD, INT, 0, strdup(yytext), yylineno, 0);
				return T_ADD; }

"*"		  	  { /* printf("%s\n",yytext); */
				yylval.INFOP = new_info(MUL, INT, 0, strdup(yytext), yylineno, 0);
				return T_MUL; }

"!"		  	  { /* printf("%s\n",yytext); */
				yylval.INFOP = new_info(NOT, BOOL, 0, strdup(yytext), yylineno, 0);
				return T_NOT; }

"||"		  { /* printf("%s\n",yytext); */
				yylval.INFOP = new_info(OR, BOOL, 0, strdup(yytext), yylineno, 0);
				return T_AND; }

"&&"		  { /* printf("%s\n",yytext); */
				yylval.INFOP = new_info(AND, BOOL, 0, strdup(yytext), yylineno, 0);
				return T_OR; }

"true"		  { /* printf("%s\n",yytext); */
				yylval.INFOP = new_info(VALUE, BOOL, 1, strdup(yytext), yylineno, 0);
				return T_TRUE; }

"false"		  { /* printf("%s\n",yytext); */
				yylval.INFOP = new_info(VALUE, BOOL, 0, strdup(yytext), yylineno, 0);
				return T_FALSE; }

"int"		  { /* printf("%s\n",yytext); */
				yylval.INFOP = new_info(TYPE, INT, 0, strdup(yytext), yylineno, 0);
				return T_INT; }

"bool"		  { /* printf("%s\n",yytext); */
				yylval.INFOP = new_info(TYPE, BOOL, 0, strdup(yytext), yylineno, 0);
				return T_BOOL; }

"void"		  { /* printf("%s\n",yytext); */
				yylval.INFOP = new_info(TYPE, VOID, 0, strdup(yytext), yylineno, 0);
				return T_VOID; }

"return"	  { /* printf("%s\n",yytext); */
				yylval.INFOP = new_info(RET, ANY, 0, strdup(yytext), yylineno, 0);
				return T_RET; }

"main"		  { /* printf("%s\n",yytext); */
				yylval.INFOP = new_info(MAIN, ANY, 0, strdup(yytext), yylineno, 0);
				return T_MAIN; }

{ID}		  { /* printf("ID: %s\n",yytext); */
				yylval.INFOP = new_info(ID, ANY, 0, strdup(yytext), yylineno, 0);
				return T_ID; }

{NRO}		  { /* printf("NRO: %d\n",atoi(yytext)); */
				yylval.INFOP = new_info(VALUE, INT, atoi(yytext), strdup(yytext), yylineno, 0);
				return T_NRO; }



[{}()]        { /* printf("%s\n",yytext); */
				return *yytext; }

[ \t\n]       ;  /* Ignorar espacios en blanco (pq los imprime????) */

.			  { /* printf("ERROR: %s\n",yytext); */
				yyerror(); }

%%

void yyerror(){
	printf("-> SYNTAX ERROR at: %d\n", yylineno);
	int line_len = strlen(itos(yylineno));
	printf("  %d  | %s\n", yylineno, yytext);
	printf("%s|\n", repeat_char(' ', line_len + 4));
}

int main(int argc,char *argv[]){
	++argv,--argc;
	if (argc > 0)
		yyin = fopen(argv[0],"r");
	else
		yyin = stdin;

	yyparse();

}
