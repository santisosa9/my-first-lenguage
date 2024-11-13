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

