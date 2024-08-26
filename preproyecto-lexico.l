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


"||"                          { printf("%s\n",yytext); return T_OP_CONJUNCION;}    

"&&"                          { printf("%s\n",yytext); return T_OP_DISYUNCION;}  

"true"                        { printf("%s\n",yytext); return T_TRUE;}  

"false"                       { printf("%s\n",yytext); return T_FALSE;}  

"int"                         { printf("%s\n",yytext); return T_INT;}  

"bool"                        { printf("%s\n",yytext); return T_BOOL;}  

"void"                        { printf("%s\n",yytext); return T_VOID;}  

"return"                      { printf("%s\n",yytext); return T_RETURN;}  

"main"                        { printf("%s\n",yytext); return T_MAIN;}  

{ID}                          { printf("ID: %s\n",yytext); return T_ID;}

{NRO}                         { printf("NRO: %d\n",atoi(yytext)); return T_NRO;}

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
