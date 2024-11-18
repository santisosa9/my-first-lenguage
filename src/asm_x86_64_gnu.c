#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../headers/asm_x86_64_gnu.h"
#include "../headers/asm_x86_64_templates.h"
#include "../headers/quadruple.h"
#include "../headers/utils.h"

char* _get_offset(Info* info, const char* reg);
char* _to_asm(Info* info);

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

            case LESS:
            case EQUAL:
            case GREATER: {
                gen_x86_64_comparison(quad, output);
                break;
            }

            case FN_DEC: {
                gen_x86_64_fn_dec(quad, output);
                break;
            }

            case LABEL: {
                gen_x86_64_label(quad, output);
                break;
            }

            case ASIG: {
                gen_x86_64_assign(quad, output);
                break; 
            }

            case JUMP: {
                gen_x86_64_jump(quad, output);
                break;
            }

            case IFNOT: {
                gen_x86_64_ifnot(quad, output);
                char* arg1 = as_info_base(quad->arg1)->props->name;
                char* result = as_info_base(quad->result)->props->name;
                const char* op = "jne";  // This will be changed, i need to get the operation
                const char* label= as_info_base(quad->result)->props->name;
                fprintf(output, "\tcmp \t%s, %s\n", arg1, result);   // Comparison line
                fprintf(output, "\t%s \t%s\n", op, label);           // Conditional jump line

                break;
            }


            case AND:
            case OR: {
                gen_x86_64_bin_boolean(quad, output);
                break;
            }

            case PARAMETER: {
                gen_x86_64_parameter(quad, output);
                // fprintf(output, "push %s\n", as_info_base(quad->result)->props->name);
                break;
            }

            case FN_CALL: {
                gen_x86_64_fn_call(quad, output);
                break;
            }

            case FN_END: {
                gen_x86_64_fn_end(quad, output);
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

    char* generated_asm = template_bin_op_result_x86_64(
                            op,
                            _to_asm(quad->arg1),
                            _to_asm(quad->arg2),
                            _to_asm(quad->result)
                        );

    fprintf(output, "%s", generated_asm);

    free(generated_asm);

}

void gen_x86_64_comparison(Quadruple* quad, FILE* output) {
    assert_pre(quad != NULL, "gen_x86_64_comparison: Invalid call error.", "'quad' must not be NULL.");
    assert_pre(output != NULL, "gen_x86_64_comparison: Invalid call error.", "'output' must not be NULL.");

    char* op = NULL;
    switch (quad->op) {
        case LESS: op = "cmovl"; break;
        case EQUAL: op = "cmove"; break;
        case GREATER: op = "cmovg"; break;

        default: {
            fprintf(stderr, "Error: gen_x86_64_comparison: Invalid quadruple error.\n");
            fprintf(stderr, "Precondition fault: 'quad' must be a valid Quadruple.\n");
            exit(EXIT_FAILURE);
        }
    }

    char* generated_asm = template_comparison_x86_64(
                            op,
                            _to_asm(quad->arg1),
                            _to_asm(quad->arg2),
                            _to_asm(quad->result)
                        );

    fprintf(output, "%s", generated_asm);

    free(generated_asm);

}

void gen_x86_64_label(Quadruple* quad, FILE* output) {
    assert_pre(quad != NULL, "gen_x86_64_label: Invalid call error.", "'quad' must not be NULL.");
    assert_pre(output != NULL, "gen_x86_64_label: Invalid call error.", "'output' must not be NULL.");

    char* generated_asm = template_label_x86_64(as_info_base(quad->result)->props->name);

    fprintf(output, "%s", generated_asm);

    free(generated_asm);
}

char* _get_offset(Info* info, const char* reg) {
    char* offset = itos(as_info_base(info)->props->offset);
    char* result = (char*) malloc(strlen(offset) + 5);
    sprintf(result, "%s(%s)", offset, reg);

    free(offset);
    return result;
}

char* _to_asm(Info* info) {
    if (as_info_base(info)->scope == LITERAL) {
        char* value = itos(as_info_base(info)->props->value);
        char* tmp = (char*) malloc(strlen("$") + strlen(value) + 1);
        sprintf(tmp, "$%s", value);

        free(value);

        return tmp;
    }
    if (as_info_base(info)->scope == GLOBAL) {
        char* result = _get_offset(info, "rip");
        return result;
    }
    if (as_info_base(info)->scope == LOCAL || as_info_base(info)->scope == PARAM) {
        char* result = _get_offset(info, "rbp");
        return result;
    }

    assert_pre(false, "_to_asm: Invalid call error.", "Invalid scope.");
}

void gen_x86_64_bin_boolean(Quadruple* quad, FILE* output) {
    assert_pre(quad != NULL, "gen_x86_64_bin_boolean: Invalid call error.", "'quad' must not be NULL.");
    assert_pre(output != NULL, "gen_x86_64_bin_boolean: Invalid call error.", "'output' must not be NULL.");

    char* absorbent = NULL;
    switch (quad->op) {
        case AND: absorbent = "$0"; break;
        case OR:  absorbent = "$1"; break;

        default: {
            fprintf(stderr, "Error: gen_x86_64_bin_logic: Invalid quadruple error.\n");
            fprintf(stderr, "Precondition fault: 'quad' must be a valid Quadruple.\n");
            exit(EXIT_FAILURE);
        }
    }

    char* comment = template_dbg_comment_x86_64("%s %s %s", as_info_base(quad->arg1)->props->name, quad->op == AND ? "&&" : "||", as_info_base(quad->arg2)->props->name);
    fprintf(output, "%s", comment);
    free(comment);
    comment = template_dbg_comment_x86_64("%s %s %s", _to_asm(quad->arg1), quad->op == AND ? "&&" : "||", _to_asm(quad->arg2));
    fprintf(output, "%s", comment);

    char* generated_asm = template_bin_boolean_x86_64(
                            absorbent,
                            _to_asm(quad->arg1),
                            _to_asm(quad->arg2),
                            _to_asm(quad->result)
                        );

    fprintf(output, "%s", generated_asm);

    free(generated_asm);
    free(comment);
}

void gen_x86_64_parameter(Quadruple* quad, FILE* output) {
    assert_pre(quad != NULL, "gen_x86_64_parameter: Invalid call error.", "'quad' must not be NULL.");
    assert_pre(output != NULL, "gen_x86_64_parameter: Invalid call error.", "'output' must not be NULL.");

    char* generated_asm = template_parameter_x86_64(
                            as_info_base(quad->result)->props->name,
                            as_info_base(quad->arg1)->props->name
                        );

    fprintf(output, "%s", generated_asm);

    free(generated_asm);
}

void gen_x86_64_ifnot(Quadruple* quad, FILE* output) {
    assert_pre(quad != NULL, "gen_x86_64_ifnot: Invalid call error.", "'quad' must not be NULL.");
    assert_pre(output != NULL, "gen_x86_64_ifnot: Invalid call error.", "'output' must not be NULL.");

    char* generated_asm = template_ifnot_x86_64(
                            _to_asm(quad->arg1),
                            as_info_base(quad->result)->props->name);

    fprintf(output, "%s", generated_asm);

    free(generated_asm);
}

void gen_x86_64_fn_dec(Quadruple* quad, FILE* output) {
    assert_pre(quad != NULL, "gen_x86_64_fn_dec: Invalid call error.", "'quad' must not be NULL.");
    assert_pre(output != NULL, "gen_x86_64_fn_dec: Invalid call error.", "'output' must not be NULL.");

    char* name = as_info_fn(quad->result)->props->name;
    int locals = as_info_fn(quad->result)->locals;

    char* generated_asm = template_fn_dec_x86_64(name, itos(locals));

    fprintf(output, "%s", generated_asm);

    free(generated_asm);
}

void gen_x86_64_fn_end(Quadruple* quad, FILE* output) {
    assert_pre(quad != NULL, "gen_x86_64_fn_end: Invalid call error.", "'quad' must not be NULL.");
    assert_pre(output != NULL, "gen_x86_64_fn_end: Invalid call error.", "'output' must not be NULL.");

    char* generated_asm = template_fn_end_x86_64();

    fprintf(output, "%s", generated_asm);

    free(generated_asm);
}

void gen_x86_64_jump(Quadruple* quad, FILE* output) {
    assert_pre(quad != NULL, "gen_x86_64_jump: Invalid call error.", "'quad' must not be NULL.");
    assert_pre(output != NULL, "gen_x86_64_jump: Invalid call error.", "'output' must not be NULL.");

    char* label = as_info_base(quad->result)->props->name;

    char* generated_asm = template_jump_x86_64(label);

    fprintf(output, "%s", generated_asm);

    free(generated_asm);
}

void gen_x86_64_assign(Quadruple* quad, FILE* output) {
    assert_pre(quad != NULL, "gen_x86_64_assign: Invalid call error.", "'quad' must not be NULL.");
    assert_pre(output != NULL, "gen_x86_64_assign: Invalid call error.", "'output' must not be NULL.");

    char* generated_asm = template_assign_x86_64(as_info_base(quad->arg1)->props->name, as_info_base(quad->result)->props->name);

    fprintf(output, "%s", generated_asm);

    free(generated_asm);
}

void gen_x86_64_fn_call(Quadruple* quad, FILE* output) {
    assert_pre(quad != NULL, "gen_x86_64_fn_call: Invalid call error.", "'quad' must not be NULL.");
    assert_pre(output != NULL, "gen_x86_64_fn_call: Invalid call error.", "'output' must not be NULL.");

    char* name = as_info_fn(quad->arg1)->props->name;
    int cant_params = atoi(as_info_base(quad->arg2)->props->name);
    char* generated_asm = template_fn_call_x86_64(name, cant_params);

    fprintf(output, "%s", generated_asm);

    free(generated_asm);
}