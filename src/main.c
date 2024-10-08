#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/ast.h"
#include "../headers/symbol_table.h"
#include "../headers/utils.h"
#include "../headers/parser.h"
#include <stdbool.h>

extern AST *get_parsed_tree();
extern int yyparse();
extern FILE *yyin;

int main(int argc, char const *argv[])
{
    ++argv, --argc;
    if (argc > 0)
        yyin = fopen(argv[0], "r");
    else
        yyin = stdin;

    yyparse();

    AST *tree = get_parsed_tree();
    SymbolTable* table = new_symbol_table();
    print_tree(tree);
    if (!fill_table(tree, table)) exit(EXIT_FAILURE);
    if (!check_types(tree, table)) exit(EXIT_FAILURE);
    // interpret(tree, table);
    // free_table(table);
    print_table(table);
    // print_tree(tree);
}
