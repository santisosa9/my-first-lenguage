#pragma once

#include <stdio.h>

#define ASM_FILE_EXTENSION ".s"
#define IDENT "\t"


// Template #1
char* template_1_x86_64(const char* op, const char* arg1, const char* arg2, const char* result);
char* template_2_x86_64(const char* label);
char* template_comparison_x86_64(const char* op, const char* arg1, const char* arg2, const char* result);
char* template_assign_x86_64(const char* arg1, const char* result);
char* template_bin_boolean_x86_64(const char* absorbent, const char* arg1, const char* arg2, const char* result);
char* template_dbg_comment_x86_64(const char* fmt, ...);
char* template_parameter_x86_64(const char* param, int index);

