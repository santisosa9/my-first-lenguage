#pragma once

#include "tag.h"
#include "info.h"

typedef struct Quadruple {
    Tag op;
    Info* arg1;
    Info* arg2;
    Info* result;
} Quadruple;

Quadruple* new_quadruple(Tag op, Info* arg1, Info* arg2, Info* result);

void print_quadruple(Quadruple* quad);