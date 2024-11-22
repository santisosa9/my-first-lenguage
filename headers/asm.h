#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "linked_list.h"

typedef enum AsmTarget {
    X86_64_LINUX_GNU,
    X86_64_LINUX_NASM
} AsmTarget;

void generate_asm(AsmTarget target, LinkedListIterator *it, FILE *f);
