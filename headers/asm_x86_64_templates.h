#pragma once

#include <stdio.h>

#define ASM_FILE_EXTENSION ".s"
#define IDENT "\t"


// Template #1
char* template_1_x86_64(const char* op, const char* arg1, const char* arg2, const char* result);
char* template_2_x86_64(const char* label);
