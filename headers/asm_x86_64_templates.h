#pragma once

#include <stdio.h>

#define ASM_FILE_EXTENSION ".s"
#define IDENT "\t"


// Template #1
char* template_bin_op_result_x86_64(const char* op, const char* arg1, const char* arg2, const char* result);
char* template_label_x86_64(const char* label);
char* template_comparison_x86_64(const char* op, const char* arg1, const char* arg2, const char* result);
char* template_assign_x86_64(const char* arg1, const char* result);
char* template_bin_boolean_x86_64(const char* absorbent, const char* arg1, const char* arg2, const char* result);
char* template_dbg_comment_x86_64(const char* fmt, ...);
char* template_parameter_x86_64(const char* param, int index);
char* template_ifnot_x86_64(const char* arg1, const char* label);
char* template_fn_dec_x86_64(const char* name, const char* locals);
char* template_fn_end_x86_64();
char* template_jump_x86_64(const char* label);
char* template_assign_x86_64(const char* arg1, const char* result);
char* template_fn_call_x86_64(const char* name, int cant_params);