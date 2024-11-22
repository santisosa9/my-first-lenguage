#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../headers/linked_list.h"

/* Returns a new LinkedListNode initialized with data. */
LinkedListNode* _new_linked_list_node(void* data)
{
    LinkedListNode* node = (LinkedListNode*)malloc(sizeof(LinkedListNode));
    node->data = data;
    node->next = NULL;
    return node;
}

LinkedList* new_linked_list()
{
    LinkedList* list = (LinkedList*)malloc(sizeof(LinkedList));
    list->size = 0;
    list->head = NULL;
    return list;
}

void insert_at(LinkedList* list, void* data, nat index)
{
    if (index > list->size)
    {
        printf("Index out of bounds\n");
        exit(1);
    }
    LinkedListNode* node = _new_linked_list_node(data);
    if (index == 0)
    {
        node->next = list->head;
        list->head = node;
    }
    else
    {
        LinkedListNode* current = list->head;
        for (nat i = 0; i < index - 1; i++)
        {
            current = current->next;
        }
        node->next = current->next;
        current->next = node;
    }
    list->size++;
}

void insert_ll(LinkedList* list, void* data)
{
    insert_at(list, data, list->size);
}

void insert_head(LinkedList* list, void* data)
{
    insert_at(list, data, 0);
}

void* remove_at(LinkedList* list, nat index)
{
    if (index >= list->size)
    {
        return NULL;
    }
    LinkedListNode* current = list->head;
    LinkedListNode* previous = NULL;
    for (nat i = 0; i < index; i++)
    {
        previous = current;
        current = current->next;
    }
    if (previous == NULL)
    {
        list->head = current->next;
    }
    else
    {
        previous->next = current->next;
    }
    void* data = current->data;
    free(current);
    if (list->size > 0) list->size--;
    else {
        printf("Internal Representation Error\n");
        exit(1);
    }
    return data;
}

void* remove_head(LinkedList* list)
{
    return remove_at(list, 0);
}

void* remove_tail(LinkedList* list)
{
    return remove_at(list, list->size - 1);
}

void* lookup(LinkedList* list, nat index)
{
    if (index >= list->size)
    {
        return NULL;
    }
    LinkedListNode* current = list->head;
    for (nat i = 0; i < index; i++)
    {
        current = current->next;
    }
    return current->data;
}

void print_list(LinkedList* list, FILE* f, void (*print)(void*))
{
    LinkedListNode* current = list->head;
    fprintf(f, "[");
    while (current != NULL)
    {
        print(current->data);
        if (current->next != NULL) fprintf(f, ", ");
        current = current->next;
    }
    fprintf(f, "]\n");
}

void free_list(LinkedList* list)
{
    LinkedListNode* current = list->head;
    while (current != NULL)
    {
        LinkedListNode* next = current->next;
        free(current);
        current = next;
    }
    free(list);
}

LinkedListIterator* new_linked_list_iterator(LinkedList* list)
{
    LinkedListIterator* iter = (LinkedListIterator*)malloc(sizeof(LinkedListIterator));
    iter->current = list->head;
    return iter;
}

void* next(LinkedListIterator* iter)
{
    if (iter->current == NULL)
    {
        return NULL;
    }
    void* data = iter->current->data;
    iter->current = iter->current->next;
    return data;
}

bool has_next(LinkedListIterator* iter)
{
    return iter->current != NULL;
}
