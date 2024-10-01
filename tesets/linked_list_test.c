#include <stdio.h>

#include "headers/linked_list.h"

void print_string(void* data)
{
    printf("%s", (char*)data);
}

int main(int argc, char const *argv[])
{
    LinkedList* list = new_linked_list();
    insert(list, "Hello");
    insert(list, "World");
    insert(list, "!");
    print_list(list, print_string);
    remove_at(list, 1);
    print_list(list, print_string);
    print_string(lookup(list, 1));
    free_list(list);
    return 0;
}

