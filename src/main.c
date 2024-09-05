#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/ast.h"
#include "../headers/symbol_table.h"
#include "../headers/utils.h"
#include <stdbool.h>


int main(int argc, char const *argv[])
{
    Info* a_info = new_info(MAIN, INT, 20, "a", 1, 0);
    Info* b_info = new_info(MAIN, INT, 20, "b", 1, 0);
    Info* c_info = new_info(MAIN, INT, 20, "c", 1, 0);
    Info* d_info = new_info(MAIN, INT, 20, "d", 1, 0);
    Info* e_info = new_info(MAIN, INT, 20, "e", 1, 0);
    Info* f_info = new_info(MAIN, INT, 20, "f", 1, 0);
    Info* g_info = new_info(MAIN, INT, 20, "g", 1, 0);

    AST* g = new_node(g_info);
    AST* f = new_node(f_info);
    AST* e = build_root(f, e_info, g);
    AST* d = new_node(d_info);
    AST* c = new_node(c_info);
    AST* b = build_root(d, b_info, e);
    AST* a = build_root(b, a_info, c);

    AST* tree = a;

    // print_tree(tree);

    SymbolTable* table = new_symbol_table();
    insert(table, a_info);
    insert(table, b_info);
    insert(table, c_info);
    insert(table, d_info);
    insert(table, e_info);
    insert(table, f_info);
    insert(table, g_info);

    printf("table: %i\n", table->size);
    print_table(table);

    puts("searching for a");
    print_info(search(table, "a")->info);
    erase(table, "a");
    printf("table: %i\n", table->size);
    puts("searching for a");
    SymbolTableNode* a_node = search(table, "a");
    print_info(a_node == NULL ? NULL : a_node->info);

    Info* new_b_info = new_info(MAIN, INT, 200, "b", 1, 0);
    print_info(b_info);
    print_info(new_b_info);
    puts("update b");
    update(table, new_b_info->name, new_b_info->value);
    // update(table, new_b_info);
    print_info(b_info);
    print_info(search(table, "b")->info);
    free_table(table);
    print_info(b_info);
    free_tree(tree);
    print_info(b_info == NULL ? NULL : b_info);

    return 0;
}
