#include <stdio.h>
#include <stdlib.h>

#include "../headers/asm_x86_64_gnu.h"
#include "../headers/asm_x86_64_templates.h"
#include "../headers/quadruple.h"
#include "../headers/utils.h"



void gen_x86_64(LinkedListIterator* it, FILE* output) {
    assert_pre(it != NULL, "generate_x86_64: Invalid call error.", "'it' must not be NULL.");
    assert_pre(output != NULL, "generate_x86_64: Invalid call error.", "'output' must not be NULL.");

    Quadruple* quad = NULL;
    while (has_next(it)) {
        quad = next(it);
        switch (quad->op) {
            case ADD:
            case SUB:
            case MUL:
            case DIV:
            case MOD: {
                gen_x86_64_bin_arith(quad, output);
                break;
            }
            case LABEL: {
                gen_x86_64_label(quad, output);
                break;
            }

            case PARAMETER: {
                fprintf(output, "push %s\n", as_info_base(quad->arg2)->props->name);
                break;
            }

            case FN_CALL: {
                fprintf(output, "call %s\n", as_info_base(quad->arg1)->props->name);
                break;
            }

            case FN_END: {
                fprintf(output, "ret\n");
                break;
            }

            default: {
                // fprintf(stderr, "Error: generate_x86_64: Invalid quadruple error.\n");
                // fprintf(stderr, "Precondition fault: 'quad' must be a valid Quadruple.\n");
                // exit(EXIT_FAILURE);
            }
        }
    }
}

void gen_x86_64_bin_arith(Quadruple* quad, FILE* output) {
    assert_pre(quad != NULL, "gen_x86_64_bin_arith: Invalid call error.", "'quad' must not be NULL.");
    assert_pre(output != NULL, "gen_x86_64_bin_arith: Invalid call error.", "'output' must not be NULL.");

    char* op = NULL;
    switch (quad->op) {
        case ADD: op = "addl";   break;
        case SUB: op = "subl";   break;
        case MUL: op = "imul";  break;
        case DIV: op = "idiv";  break;
        case MOD: op = "idiv";  break;

        default: {
            fprintf(stderr, "Error: gen_x86_64_bin_arith: Invalid quadruple error.\n");
            fprintf(stderr, "Precondition fault: 'quad' must be a valid Quadruple.\n");
            exit(EXIT_FAILURE);
        }
    }

    char* generated_asm = template_1_x86_64(
                            op,
                            as_info_base(quad->arg1)->props->name,
                            as_info_base(quad->arg2)->props->name,
                            as_info_base(quad->result)->props->name
                        );

    fprintf(output, "%s", generated_asm);

    free(generated_asm);

}


void gen_x86_64_label(Quadruple* quad, FILE* output) {
    assert_pre(quad != NULL, "gen_x86_64_label: Invalid call error.", "'quad' must not be NULL.");
    assert_pre(output != NULL, "gen_x86_64_label: Invalid call error.", "'output' must not be NULL.");

    char* generated_asm = template_2_x86_64(as_info_base(quad->arg1)->props->name);

    fprintf(output, "%s", generated_asm);

    free(generated_asm);
}
