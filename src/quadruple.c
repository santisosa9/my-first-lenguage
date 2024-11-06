#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../headers/quadruple.h"
#include "../headers/utils.h"

Quadruple* new_quadruple(Tag op, Info* arg1, Info* arg2, Info* result) {
    Quadruple* quad = (Quadruple*)malloc(sizeof(Quadruple));
    quad->op = op;
    quad->arg1 = arg1;
    quad->arg2 = arg2;
    quad->result = result;
    return quad;
}

void print_quadruple(Quadruple* quad) {
    if (quad == NULL) {
        puts("");
        return;
    }

    char* op = tag_to_str(quad->op);
    // char* padded_op = (char*)malloc(11 * sizeof(char));
    // snprintf(padded_op, 11, "%-10s", op);

    printf("{%-10s" , op);
    printf(", %-10s", (quad->arg1 && as_info_base(quad->arg1)->props) ? as_info_base(quad->arg1)->props->name : " ");
    printf(", %-10s", (quad->arg2 && as_info_base(quad->arg2)->props) ? as_info_base(quad->arg2)->props->name : " ");
    printf(", %-10s", (quad->result && as_info_base(quad->result)->props) ? as_info_base(quad->result)->props->name : " ");
    printf("}\n");
    free(op);
}
