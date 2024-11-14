#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "../headers/asm_x86_64_templates.h"

/*
    Template #1

    Assign the result of a binary operation to a variable.

    result = arg1 op arg2

    It use 'movq' to move data.
    It use '%r10' as a temporary register.

    Important: Need to free the returned string.
 */
char* template_1_x86_64(const char* op, const char* arg1, const char* arg2, const char* result) {
    const char* template =
        IDENT "movq" IDENT "%s, %%r10\n"    // movl arg1, %r10
        IDENT "%s"   IDENT "%s, %%r10\n"    // op   arg2, %r10
        IDENT "movq" IDENT "%%r10, %s\n";   // movl %r10, result

    char* buffer = (char*) malloc(strlen(template) + strlen(op) + strlen(arg1) + strlen(arg2) + strlen(result) + 1);

    sprintf(buffer, template, arg1, op, arg2, result);

    return buffer;
}

/*
    Template #2

    Assign the result of a binary operation to a variable.

    label:

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


char* template_bin_boolean_x86_64(const char* absorbent, const char* arg1, const char* arg2, const char* result) {
    const char* template =
        IDENT "movq"    IDENT "%s, %%r10\n"     // movq     arg1, %r10
        IDENT "cmpq"    IDENT "%%r10, %s\n"     // cmpq     %r10, absorbent
        IDENT "cmove"   IDENT "%s, %%r11\n"     // move     absorbent, %r11
        IDENT "movq"    IDENT "%s, %%r10\n"     // movq     arg2, %r10
        IDENT "cmovne"  IDENT "%%r10, %%r11\n"  // cmovne   %r10, %r11
        IDENT "movq"    IDENT "%%r11, %s\n\n";  // movq     %r11, result

    char* buffer = (char*) malloc(strlen(template) + strlen(absorbent) + strlen(arg1) + strlen(arg2) + strlen(result) + 1);

    sprintf(buffer, template, arg1, absorbent, absorbent, arg2, result);

    return buffer;
}


char* template_dbg_comment_x86_64(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    // Calculate the length of the formatted string
    int len = vsnprintf(NULL, 0, fmt, args);
    va_end(args);

    // Allocate memory for the formatted string
    char* comment = (char*) malloc(len + 1);
    if (!comment) {
        return NULL;
    }

    // Format the string
    va_start(args, fmt);
    vsnprintf(comment, len + 1, fmt, args);
    va_end(args);

    const char* template =
        IDENT "# %s\n";

    char* buffer = (char*) malloc(strlen(template) + strlen(comment) + 1);
    if (!buffer) {
        free(comment);
        return NULL;
    }

    sprintf(buffer, template, comment);
    free(comment);

    return buffer;
}

