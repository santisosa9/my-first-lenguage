#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../headers/asm.h"
#include "../headers/asm_x86_64_gnu.h"
#include "../headers/asm_x86_64_templates.h"
#include "../headers/intermediate_code.h"
#include "../headers/quadruple.h"
#include "../headers/utils.h"

void generate_asm(AsmTarget target, LinkedListIterator* ic, FILE* output) {
    switch (target) {
        case X86_64_LINUX_GNU: {
            gen_x86_64(ic, output);
            break;
        }

        default: {
            fprintf(stderr, "Error: generate_asm: Invalid target error.\n");
            fprintf(stderr, "Precondition fault: 'target' must be a valid AsmTarget.\n");
            exit(EXIT_FAILURE);
        }
    }
}


