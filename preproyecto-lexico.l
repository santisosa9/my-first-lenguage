%{
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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

NRO             (-?[1-9][0-9]{0,4})|0


/* Reglas */

%%


"||"                          { printf("%s\n",yytext); return OP_CONJUNCION;}    

"&&"                          { printf("%s\n",yytext); return OP_DISYUNCION;}  

"true"                        { printf("%s\n",yytext); return TRUE_CTE;}  

"false"                       { printf("%s\n",yytext); return FALSE_CTE;}  

"int"                         { printf("%s\n",yytext); return INT;}  

"bool"                        { printf("%s\n",yytext); return BOOL;}  

"void"                        { printf("%s\n",yytext); return VOID;}  

"return"                      { printf("%s\n",yytext); return RETURN;}  

"main"                        { printf("%s\n",yytext); return MAIN;}  

{ID}                          { printf("ID: %s\n",yytext); return ID;}

{NRO}                         { printf("NRO: %d\n",atoi(yytext)); return NRO;}

[+*=!{};()]                   { printf("%s\n",yytext); return *yytext;}

\n                            ;

%%

void yyerror(){
	printf("%s%d\n","-> ERROR Sintactico en la linea: ", yylineno);
}

int main(int argc,char *argv[]){
	++argv,--argc;
	if (argc > 0)
		yyin = fopen(argv[0],"r");
	else
		yyin = stdin;

	yyparse();

}
