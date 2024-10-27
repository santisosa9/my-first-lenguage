#pragma once

#include <stdbool.h>
#include "ast.h"

/* Performs type checking */
bool check_types(AST* tree);

/* Check if an expression is well defined */
bool check_expr(AST* tree);

/* Check if a function is well defined */
bool check_fn(AST* fn);

/* Checks if a function is well invocated */
bool check_params(LinkedList* formal_params, AST* fn_call);
