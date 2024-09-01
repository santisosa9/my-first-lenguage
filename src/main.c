#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/ast.h"
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

    print_tree(tree);
    free_tree(tree);
    return 0;
}
