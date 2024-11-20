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
    bool data_section_emitted = false;  
    bool text_section_emitted = false;

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
                if (!text_section_emitted) {
                    fprintf(output, IDENT ".text\n");
                    text_section_emitted = true;
                }
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
                break;
            }

            case AND:
            case OR: {
                gen_x86_64_bin_boolean(quad, output);
                break;
            }

            case PARAMETER: {
                gen_x86_64_parameter(quad, output);
                break;
            }

            case VOID_FC: {
                gen_x86_64_void_fn_call(quad, output);
                break;
            }

            case TYPED_FC: {
                gen_x86_64_typed_fn_call(quad, output);
                break;
            }

            case FN_END: {
                gen_x86_64_fn_end(quad, output);
                break;
            }

            case GLOBAL_DEC: {
                if (!data_section_emitted) {
                    fprintf(output, IDENT ".data\n");
                    data_section_emitted = true;
                }
                gen_x86_64_global_dec(quad, output);
                break;
            }

            case RET: {
                gen_x86_64_ret(quad, output);
                break;
            }

            default: {

            }
        }
    }
}

void gen_x86_64_bin_arith(Quadruple* quad, FILE* output) {
    assert_pre(quad != NULL, "gen_x86_64_bin_arith: Invalid call error.", "'quad' must not be NULL.");
    assert_pre(output != NULL, "gen_x86_64_bin_arith: Invalid call error.", "'output' must not be NULL.");

    char* op = NULL;
    switch (quad->op) {
        case ADD: op = "addq";   break;
        case SUB: op = "subq";   break;
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
    if (as_info_fn(info)->scope == NO_SCOPE || as_info_base(info)->scope == LOCAL || as_info_base(info)->scope == PARAM || as_info_base(info)->scope == NO_SCOPE) {
        char* result = _get_offset(info, "%rbp");
        return result;
    }
    if (as_info_base(info)->scope == GLOBAL) {
        char* name = as_info_base(info)->props->name;
        char* result = (char*) malloc(strlen(name) + strlen("%rip") + 1);
        sprintf(result, "%s(%%rip)", name);

        return result;
    }
    assert_pre(false, "_to_asm: Invalid call error.", "Invalid scope.");
    return NULL;
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

    char* generated_asm = template_bin_boolean_x86_64(
                            absorbent,
                            _to_asm(quad->arg1),
                            _to_asm(quad->arg2),
                            _to_asm(quad->result)
                        );

    fprintf(output, "%s", generated_asm);

    free(generated_asm);
}

void gen_x86_64_parameter(Quadruple* quad, FILE* output) {
    assert_pre(quad != NULL, "gen_x86_64_parameter: Invalid call error.", "'quad' must not be NULL.");
    assert_pre(output != NULL, "gen_x86_64_parameter: Invalid call error.", "'output' must not be NULL.");

    char* generated_asm = template_parameter_x86_64(
                            _to_asm(quad->result),
                            as_info_base(quad->arg1)->props->value
                        );

    fprintf(output, "%s", generated_asm);

    free(generated_asm);
}

void gen_x86_64_ifnot(Quadruple* quad, FILE* output) {
    assert_pre(quad != NULL, "gen_x86_64_ifnot: Invalid call error.", "'quad' must not be NULL.");
    assert_pre(output != NULL, "gen_x86_64_ifnot: Invalid call error.", "'output' must not be NULL.");

    char* generated_asm = template_ifnot_x86_64(
                            _to_asm(quad->arg1),
                            as_info_base(quad->result)->props->name
                        );

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



    char* generated_asm = template_assign_x86_64(_to_asm(quad->arg1), _to_asm(quad->result));

    fprintf(output, "%s", generated_asm);

    free(generated_asm);
}

void gen_x86_64_void_fn_call(Quadruple* quad, FILE* output) {
    assert_pre(quad != NULL, "gen_x86_64_void_fn_call: Invalid call error.", "'quad' must not be NULL.");
    assert_pre(output != NULL, "gen_x86_64_void_fn_call: Invalid call error.", "'output' must not be NULL.");

    char* name = as_info_fn(quad->arg1)->props->name;
    int cant_params = atoi(as_info_base(quad->arg2)->props->name);
    char* generated_asm = template_void_fn_call_x86_64(name, cant_params);

    fprintf(output, "%s", generated_asm);

    free(generated_asm);
}

void gen_x86_64_typed_fn_call(Quadruple* quad, FILE* output) {
    assert_pre(quad != NULL, "gen_x86_64_typed_fn_call: Invalid call error.", "'quad' must not be NULL.");
    assert_pre(output != NULL, "gen_x86_64_typed_fn_call: Invalid call error.", "'output' must not be NULL.");

    char* name = as_info_fn(quad->arg1)->props->name;
    int cant_params = atoi(as_info_base(quad->arg2)->props->name);
    char* generated_asm = template_typed_fn_call_x86_64(name, cant_params, _to_asm(quad->result));

    fprintf(output, "%s", generated_asm);

    free(generated_asm);
}

void gen_x86_64_global_dec(Quadruple* quad, FILE* output) {
    assert_pre(quad != NULL, "gen_x86_64_global_dec: Invalid call error.", "'quad' must not be NULL.");
    assert_pre(output != NULL, "gen_x86_64_global_dec: Invalid call error.", "'output' must not be NULL.");

    InfoBase* result_info = as_info_base(quad->result);
    Props* props = result_info->props;

    const char* type_str = NULL;
    switch (props->type) {
        case INT:
            type_str = ".long";  // Enteros de 4 bytes
            break;
        case BOOL:
            type_str = ".byte";  // Booleanos de 1 byte
            break;
        default:
            type_str = ".zero";  // Sin tipo: reservar espacio sin inicializar
            break;
    }

    char* name = props->name;
    int align = (props->type == BOOL) ? 1 : 8;
    char* value = itos(props->value);
    char* generated_asm = template_global_dec_x86_64(name, itos(align), type_str, value);

    fprintf(output, "%s", generated_asm);

    free(generated_asm);
}

void gen_x86_64_ret(Quadruple* quad, FILE* output) {
    assert_pre(quad != NULL, "gen_x86_64_ret: Invalid call error.", "'quad' must not be NULL.");
    assert_pre(output != NULL, "gen_x86_64_ret: Invalid call error.", "'output' must not be NULL.");

    char* generated_asm = template_ret_x86_64(_to_asm(quad->result));

    fprintf(output, "%s", generated_asm);

    free(generated_asm);
}