#include <stdlib.h>
#include <string.h>

#include "../headers/asm_x86_64_templates.h"

/*
    Template #1

    Assign the result of a binary operation to a variable.

    result = arg1 op arg2

    It use 'movl' to move data.
    It use '%r10' as a temporary register.

    Important: Need to free the returned string.
 */
char* template_1_x86_64(const char* op, const char* arg1, const char* arg2, const char* result) {
    const char* template =
        IDENT "movl" IDENT "%s, %%r10\n"    // movl arg1, %r10
        IDENT "%s"   IDENT "%s, %%r10\n"    // op   arg2, %r10
        IDENT "movl" IDENT "%%r10, %s\n";   // movl %r10, result

    char* buffer = (char*) malloc(strlen(template) + strlen(op) + strlen(arg1) + strlen(arg2) + strlen(result) + 1);

    sprintf(buffer, template, arg1, op, arg2, result);

    return buffer;
}

/*
    Template #2

    Assign the result of a binary operation to a variable.

    label:

    It use 'movl' to move data.
    It use '%r10' as a temporary register.

    Important: Need to free the returned string.
 */
char* template_2_x86_64(const char* label) {
    const char* template =
        "%s:\n";

    char* buffer = (char*) malloc(strlen(template) + strlen(label) + 1);

    sprintf(buffer, template, label);

    return buffer;
}

char* template_comparison_x86_64(const char* op1, const char* op2, const char* arg1, const char* arg2, const char* result) {
    const char* template =
        IDENT "cmp"  IDENT "%s, %s\n"   // cmp arg1, arg2
        IDENT "%s"   IDENT "1, %s\n"    // op1 1, result
        IDENT "%s"   IDENT "0, %s\n";   // op2 0, result

    char* buffer = (char*) malloc(strlen(template) + strlen(arg1) + strlen(arg2) + strlen(op1) + strlen(op2) + (strlen(result) * 2) + 1);

    sprintf(buffer, template, arg1, arg2, op1, result, op2, result);

    return buffer;
}
