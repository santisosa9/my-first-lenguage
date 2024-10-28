#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../headers/ast.h"
#include "../headers/symbol_table.h"
#include "../headers/utils.h"
#include "../headers/parser.h"
#include "../headers/type_checking.h"

extern AST *get_parsed_tree();
extern int yyparse();
extern FILE *yyin;

int main(int argc, char const *argv[])
{
    ++argv, --argc;
    if (argc > 0) {
        yyin = fopen(argv[0], "r");
        if (yyin == NULL) {
            fprintf(stderr, "Cannot open file '%s'.\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    else if (!feof(stdin)) {
        yyin = stdin;
    }
    else {
        fprintf(stderr, "No input file.\n");
        exit(EXIT_FAILURE);
    }

    yyparse();

    AST *tree = get_parsed_tree();
    SymbolTable *table = new_symbol_table();

    // puts("Printing tree...");
    // print_tree(tree);
    puts("Decorating tree...");
    if (!decorate_tree(tree, table))
        exit(EXIT_FAILURE);
    puts("Printing tree...");
    print_tree(tree);
    // puts("------------------------------");
    puts("Checking types...");
    if (!check_types(tree)){
        puts(RED "fail" RESET);
        exit(EXIT_FAILURE);
    }
    puts("Generating intermediate code...");
    generate_intermediate_code(tree);    
    // puts("Printing tree...");
    // print_tree(tree);
}
