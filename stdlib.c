#include <stdio.h>
#include <stdbool.h>

void print_int(int x) {
    printf("%d\n", x);
}

void print_bool(bool x) {
    printf("%d\n", x );
    printf("%s\n", x ? "true" : "false");
}
