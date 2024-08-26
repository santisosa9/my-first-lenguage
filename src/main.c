#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "utils.h"


int main(int argc, char const *argv[])
{
    Info* info = new_info(10, INT, INT);
    AST* left = new_tree(new_info(20, INT, INT), new_node(new_info(40, INT, INT)), new_node(new_info(50, INT, INT)));
    AST* right = new_node(new_info(30, INT, INT));
    AST* tree = new_tree(info, left, right);
    print_tree(tree);
    free_tree(tree);
    return 0;
}
