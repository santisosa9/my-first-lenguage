#pragma once

#define RED     "\033[0;31m"
#define YELLOW  "\033[0;33m"
#define RESET   "\033[0m"

typedef unsigned int nat;

char* repeat_str(char* str, int times);
char* repeat_char(char chr, int times);
char* itos(int n);
char* strdup(const char* s);
char* get_file_path();