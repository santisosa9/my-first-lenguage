#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../headers/ast.h"
#include "../headers/symbol_table.h"
#include "../headers/utils.h"
#include "../headers/parser.h"
#include "../headers/type_checking.h"
#include "../headers/intermediate_code.h"
#include "../headers/quadruple.h"

extern AST *get_parsed_tree();
extern int yyparse();
extern FILE *yyin;
extern char *current_file_path;

int main(int argc, char const *argv[])
{
    ++argv, --argc;
    if (argc > 0) {
        current_file_path = strdup(argv[0]);
        yyin = fopen(current_file_path, "r");
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

    puts("Printing tree...");
    print_tree(tree);
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
    LinkedList* l = new_linked_list();
    generate_intermediate_code(tree, l);
    puts("Printing tree...");
    print_tree(tree);
    puts("Printing quadruple list...");
    print_list(l, (void (*)(void*)) print_quadruple);
}
