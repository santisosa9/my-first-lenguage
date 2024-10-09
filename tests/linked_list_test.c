#include <stdio.h>

#include "../headers/linked_list.h"

void print_string(void* data)
{
    printf("%s", (char*)data);
}

int main(int argc, char const *argv[])
{
    LinkedList* list = new_linked_list();
    insert_ll(list, "Hello");
    insert_ll(list, "World");
    insert_ll(list, "!");
    print_list(list, print_string);
    remove_at(list, 1);
    print_list(list, print_string);
    print_string(lookup(list, 1));
    insert_ll(list, "Hi");
    print_list(list, print_string);
    LinkedListIterator* iter = new_linked_list_iterator(list);
    while (has_next(iter))
    {
        printf("%s\n", (char*)next(iter));
    }

    free_list(list);
    return 0;
}

