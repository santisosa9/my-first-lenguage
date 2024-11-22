#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

#include "../headers/ast.h"
#include "../headers/symbol_table.h"
#include "../headers/utils.h"
#include "../headers/parser.h"
#include "../headers/type_checking.h"
#include "../headers/intermediate_code.h"
#include "../headers/quadruple.h"
#include "../headers/asm.h"
#include "../headers/cli.h"
#include "../headers/config.h"

extern AST *get_parsed_tree();
extern int yyparse();
extern FILE *yyin;
extern char *current_file_path;
extern Config *cfg;

int main(int argc, char const *argv[])
{
    /* Args */

    if (!parse_args(argc, argv)) {
        print_help();
        exit(EXIT_FAILURE);
    }

    if (cfg->debug) print_config(cfg);


    /* Help */

    if (cfg->help) {
        print_help();
        exit(EXIT_SUCCESS);
    }


    /* Parsing */

    current_file_path = cfg->input_path;
    yyin = fopen(current_file_path, "r");
    assert(yyin != NULL, "Cannot open file '%s'.", current_file_path);

    if (cfg->verbose) puts(INFO "Parsing...");
    yyparse();
    

    AST *tree = get_parsed_tree();
    SymbolTable *table = new_symbol_table();

    if (cfg->debug){
        puts(DEBUG "Printing tree...");
        print_tree(tree);
        puts("\n------------------------------\n");
    }

    assert(decorate_tree(tree, table), "((Internal Error)) Tree decoration failed.");

    if (cfg->debug){
        puts(DEBUG "Printing decorated tree...");
        print_tree(tree);
        puts("\n------------------------------\n");
    }

    if (cfg->target == PARSE) {
        if (cfg->verbose) printf("Writing AST tree into '%s'...", cfg->output_path);
        FILE *f = fopen(cfg->output_path, "wb");
        assert(f != NULL, "Cannot open file '%s'.", cfg->output_path);
        write_tree_to_file(tree, f);
        exit(EXIT_SUCCESS);
    }

    /* Type Checking */

    if (cfg->verbose) puts(INFO "Checking types...");
    assert(check_types(tree), "Type checking failed.");


    /* Intermediate Code Generation */

    if (cfg->verbose) puts(INFO "Generating intermediate code...");
    LinkedList* l = new_linked_list();
    generate_intermediate_code(tree, l);

    free_tree(tree);

    if (cfg->debug) {
        puts(DEBUG "Printing intermediate code...");
        print_list(l, stdout, (void (*)(void*)) print_quadruple);
        puts("\n------------------------------\n");
    }


    if (cfg->target == CODINT) {
        if (cfg->verbose) printf("Writing intermediate code into '%s'...", cfg->output_path);
        FILE *f = fopen(cfg->output_path, "w");
        assert(f != NULL, "Cannot open file '%s'.", cfg->output_path);
        print_list(l, f, (void (*)(void*)) print_quadruple);
        exit(EXIT_SUCCESS);
    }


    /* ASM Generation */

    if (cfg->verbose) puts(INFO "Generating ASM...");
    LinkedListIterator* it = new_linked_list_iterator(l);

    FILE *f;

    // if (cfg->debug && cfg->target != ASM){
    //     printf(DEBUG "Writting ASM into '%s'...\n", cfg->asm_path);
    //     f = fopen(cfg->asm_path, "w");
    //     assert(f != NULL, "Cannot open file '%s'.", cfg->asm_path);
    //     generate_asm(cfg->asm_target, it, f);
    //     fflush(f);
    //     fclose(f);
    //     puts("\n------------------------------\n");
    // }

    bool using_tmp_file = false;
    if (cfg->target != ASM && !cfg->debug) {
        using_tmp_file = true;
    }

    f = fopen(cfg->asm_path, "w");
    assert(f != NULL, "Cannot open file '%s'.", cfg->asm_path);

    generate_asm(cfg->asm_target, it, f);

    fflush(f);
    fclose(f);
    free(it);

    if (cfg->debug){
        it = new_linked_list_iterator(l);
        puts(DEBUG "Printing ASM...");
        generate_asm(X86_64_LINUX_GNU, it, stdout);
        free(it);
        puts("\n------------------------------\n");
    }

    free_list(l);

    if (cfg->target == ASM) {
        if (cfg->verbose) puts(INFO "Done.");
        exit(EXIT_SUCCESS);
    }


    /* Binary Generation */

    if(cfg->verbose) puts(INFO "Generating binary...");

    if (access("stdlib.c", F_OK) == -1) {
        write_stdlib();
    }

    char* cmd = NULL;
    if (cfg->external) {
        cmd = (char*) malloc(strlen(cfg->asm_path) + strlen(cfg->external_path) + strlen(cfg->output_path) + 40);
        sprintf(cmd, "gcc -z noexecstack stdlib.c %s %s -o %s", cfg->asm_path, cfg->external_path, cfg->output_path);
    } else {
        cmd = (char*) malloc(strlen(cfg->asm_path) + strlen(cfg->output_path) + 40);
        sprintf(cmd, "gcc -z noexecstack stdlib.c %s -o %s ", cfg->asm_path, cfg->output_path);
    }

    if (cfg->debug){
        puts(DEBUG "Assembling and generating executable with gcc...");
        printf(DEBUG "Running: %s\n", cmd);
    }

    system(cmd);
    free(cmd);
    if (using_tmp_file) unlink(cfg->asm_path);

    if (cfg->verbose) puts(INFO "Done.");
    free_config(cfg);

}
