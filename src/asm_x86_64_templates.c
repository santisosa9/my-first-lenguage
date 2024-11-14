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

char* template_comparison_x86_64(const char* op, const char* arg1, const char* arg2, const char* result) {
    const char* template =
        IDENT "movq"  IDENT "%s, %%r10\n"
        IDENT "movq"  IDENT "%s, %%r11\n"
        IDENT "cmp"   IDENT "%%r10, %%r11\n"   
        IDENT "movq"  IDENT "$0, %%r11\n"
        IDENT "movq"  IDENT "$1, %%r10\n"
        IDENT "%s"    IDENT "%%r10, %%r11\n"    
        IDENT "movq"  IDENT "%%r11, %s\n\n";

    char* buffer = (char*) malloc(strlen(template) + strlen(arg1) + strlen(arg2) + strlen(op) + strlen(result) + 1);

    sprintf(buffer, template, arg1, arg2, op, result);

    return buffer;
}

/*
    Template #3

    Assign a value to a variable.

    label:
    
    - Uses 'movq' to move data.
    - Uses '%r10' as a temporary register to hold the value to be assigned.

    Important: Need to free the returned string.
 */
char* template_3_x86_64(const char* arg1, const char* result) {
    const char* template =
        IDENT "movl" IDENT  "%s, %%r10\n"      //Move the immediate value `arg1` to %r10
        IDENT "movl" IDENT  "%%r10, %s\n";     //Move the value in %r10 to `result`

        char* buffer = (char*) malloc(strlen(template) + strlen(arg1) + strlen(result) +1);

    sprintf(buffer, template, arg1, result);

    return buffer;
}


