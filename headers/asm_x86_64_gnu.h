#pragma once

#include <stdio.h>
#include "quadruple.h"
#include "linked_list.h"

void gen_x86_64(LinkedListIterator* it, FILE* output);
void gen_x86_64_bin_arith(Quadruple* quad, FILE* output);
void gen_x86_64_label(Quadruple* quad, FILE* output);
void gen_x86_64_bin_boolean(Quadruple* quad, FILE* output);
