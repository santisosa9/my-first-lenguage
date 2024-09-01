%{
	#include <stdlib.h>
	#include <string.h>
	#include <stdio.h>

	#include "headers/ast.h"
	#include "headers/utils.h"
	#include "preproyecto-sintaxis.tab.h"
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

";" 		  { printf("%s\n",yytext);
				yylval.INFOP = new_info(COMMA, NO_TYPED, NULL, strdup(yytext), yylineno, 0);
				return T_COMMA; }

"="			  { printf("%s\n",yytext);
				yylval.INFOP = new_info(ASIG, NO_TYPED, NULL, strdup(yytext), yylineno, 0);
				return T_ASIG; }

"+"		  	  { printf("%s\n",yytext);
				yylval.INFOP = new_info(ADD, NO_TYPED, NULL, strdup(yytext), yylineno, 0);
				return T_ADD; }

"*"		  	  { printf("%s\n",yytext);
				yylval.INFOP = new_info(MUL, NO_TYPED, NULL, strdup(yytext), yylineno, 0);
				return T_MUL; }

"!"		  	  { printf("%s\n",yytext);
				yylval.INFOP = new_info(NOT, NO_TYPED, NULL, strdup(yytext), yylineno, 0);
				return T_NOT; }

"||"		  { printf("%s\n",yytext);
				yylval.INFOP = new_info(OR, NO_TYPED, NULL, strdup(yytext), yylineno, 0);
				return T_AND; }

"&&"		  { printf("%s\n",yytext);
				yylval.INFOP = new_info(AND, NO_TYPED, NULL, strdup(yytext), yylineno, 0);
				return T_OR; }

"true"		  { printf("%s\n",yytext);
				yylval.INFOP = new_info(VALUE, BOOL, NULL, strdup(yytext), yylineno, 0);
				return T_TRUE; }

"false"		  { printf("%s\n",yytext);
				yylval.INFOP = new_info(VALUE, BOOL, NULL, strdup(yytext), yylineno, 0);
				return T_FALSE; }

"int"		  { printf("%s\n",yytext);
				yylval.INFOP = new_info(TYPE, NO_TYPED, NULL, strdup(yytext), yylineno, 0);
				return T_INT; }

"bool"		  { printf("%s\n",yytext);
				yylval.INFOP = new_info(TYPE, NO_TYPED, NULL, strdup(yytext), yylineno, 0);
				return T_BOOL; }

"void"		  { printf("%s\n",yytext);
				yylval.INFOP = new_info(TYPE, NO_TYPED, NULL, strdup(yytext), yylineno, 0);
				return T_VOID; }

"return"	  { printf("%s\n",yytext);
				yylval.INFOP = new_info(RET, ANY, NULL, strdup(yytext), yylineno, 0);
				return T_RET; }

"main"		  { printf("%s\n",yytext);
				yylval.INFOP = new_info(MAIN, ANY, NULL, strdup(yytext), yylineno, 0);
				return T_MAIN; }

{ID}		  { printf("ID: %s\n",yytext);
				yylval.INFOP = new_info(ID, ANY, NULL, strdup(yytext), yylineno, 0);
				return T_ID; }

{NRO}		  { printf("NRO: %d\n",atoi(yytext));
				yylval.INFOP = new_info(VALUE, INT, atoi(yytext), strdup(yytext), yylineno, 0);
				return T_NRO; }



[{}()]        { printf("%s\n",yytext);
				return *yytext; }

\n             ;

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
