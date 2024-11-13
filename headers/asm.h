#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "linked_list.h"

typedef enum ASM_TARGET {
    X86_64_LINUX_GNU,
    X86_64_LINUX_NASM
} ASM_TARGET;

void generate_asm(ASM_TARGET target, LinkedListIterator *it, FILE *f);
