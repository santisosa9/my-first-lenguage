%{
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "calc-sintaxis.tab.h"
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

TRUE            "true"
FALSE           "false"

CTE             {NRO}|{TRUE}|{FALSE}

INT             "int"
BOOL            "bool"
VOID            "void"

/* Reglas */

%%

{NRO}                         { printf("number: %d\n",atoi(yytext)); return NRO;}

{ID}                          { printf("id: %s\n",yytext); return ID;}

"+"                           { printf("%s\n",yytext); return OP_SUMA;} 

"*"                           { printf("%s\n",yytext); return OP_MULTI;}  

"="                           { printf("%s\n",yytext); return OP_IGUAL;}    

"!"                           { printf("%s\n",yytext); return OP_NEGACION;}

"||"                          { printf("%s\n",yytext); return OP_CONJUNCION;}    

"&&"                          { printf("%s\n",yytext); return OP_DISYUNCION;}  

"{"                           { printf("%s\n",yytext); return LLAVE_A;}

"}"                           { printf("%s\n",yytext); return LLAVE_C;}

"("                           { printf("%s\n",yytext); return PARENTESIS_A;}

")"                           { printf("%s\n",yytext); return PARENTESIS_C;}

";"                           { printf("%s\n",yytext); return FIN_SENTENCIA;}

"main()"                      ;

[+*=!{};()]                   { printf("%s\n",yytext); return *yytext;}

.|\n                          ; /* ignore all the rest */

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
