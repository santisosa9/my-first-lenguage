#pragma once

#include "ast.h"
#include "linked_list.h"

void generate_intermediate_code(AST* tree, LinkedList* intermediate_code);