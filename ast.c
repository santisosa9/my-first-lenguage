#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "utils.h"

void _print_tree(AST* tree, int level);


AST* new_tree(Info* info, AST* left, AST* right){
    AST* tree = (AST*)malloc(sizeof(AST));
    tree->info = info;
    tree->left = left;
    tree->right = right;
    return tree;
}

AST* new_node(Info* info){
    return new_tree(info, NULL, NULL);
}

Info* new_info(int value, Tag tag, Type type){
    Info* info = (Info*)malloc(sizeof(Info));
    info->value = value;
    info->tag = tag;
    info->type = type;
    return info;
}

void free_tree(AST* tree){
    if(tree == NULL) return;
    free_tree(tree->left);
    free_tree(tree->right);
    free(tree->info);
    free(tree);
}

void print_tree(AST* tree){
    _print_tree(tree, 0);
}

void _print_tree(AST* tree, int level){
    if(tree == NULL) return;
    printf("%s", repeat_str("   ", level));
    print_info(tree->info);
    _print_tree(tree->left, level+1);
    _print_tree(tree->right, level+1);
}

void print_info(Info* info){
    printf("Value: %d ", info->value);
    printf("Tag: %d ", info->tag);
    printf("Type: %d\n", info->type);
}
