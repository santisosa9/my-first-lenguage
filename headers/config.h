#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "asm.h"

typedef enum
{
    PARSE,
    CODINT,
    ASM,
    BIN
} Target;


typedef struct Config
{
    bool help;
    bool verbose;
    bool debug;
    bool output;
    bool external;
    char* output_path;
    char* input_path;
    char* input_base_name;
    char* external_path;
    char* asm_path;
    Target target;
    AsmTarget asm_target;
} Config;

Config* new_config();

void free_config(Config* config);

void print_config(Config* config);

void write_stdlib();
