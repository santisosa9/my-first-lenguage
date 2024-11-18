#pragma once

#include <stdio.h>
#include "quadruple.h"
#include "linked_list.h"

void gen_x86_64(LinkedListIterator* it, FILE* output);
void gen_x86_64_bin_arith(Quadruple* quad, FILE* output);
void gen_x86_64_label(Quadruple* quad, FILE* output);
void gen_x86_64_bin_boolean(Quadruple* quad, FILE* output);
void gen_x86_64_comparison(Quadruple* quad, FILE* output);
void gen_x86_64_parameter(Quadruple* quad, FILE* output);
void gen_x86_64_ifnot(Quadruple* quad, FILE* output);
void gen_x86_64_fn_dec(Quadruple* quad, FILE* output);
void gen_x86_64_fn_end(Quadruple* quad, FILE* output);
void gen_x86_64_jump(Quadruple* quad, FILE* output);
void gen_x86_64_assign(Quadruple* quad, FILE* output);
void gen_x86_64_fn_call(Quadruple* quad, FILE* output);