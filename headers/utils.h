#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <regex.h>

#define RED     "\033[0;31m"
#define YELLOW  "\033[0;33m"
#define RESET   "\033[0m"

typedef unsigned int nat;

typedef struct {
    FILE *file;
    char *filename;
} TmpFile;

char* repeat_str(char* str, int times);
char* repeat_char(char chr, int times);
char* itos(int n);
char* strdup(const char* s);
char* get_file_path();

void assert_pre(bool condition, const char* error_msg, const char* precondition_fault_msg);
void assert(bool condition, const char* error_msg, ...);

regex_t *new_regex(const char *pattern);
bool match(regex_t *regex, const char *str);
bool match_sub(regex_t *regex, const char *str, size_t nmatch, regmatch_t pmatch[]);
char* get_substring(const char *str, regmatch_t pmatch);

TmpFile new_tmp_file();
