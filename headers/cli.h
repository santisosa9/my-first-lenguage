#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "asm.h"
// #include "config.h"

#define INFO    "[info]  "
#define DEBUG   "[debug] "

bool parse_args(int argc, char const *argv[]);
void print_help();
