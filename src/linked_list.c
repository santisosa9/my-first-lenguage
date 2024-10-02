#include <stdio.h>
#include <stdlib.h>
#include "../headers/linked_list.h"

LinkedListNode* new_linked_list_node(void* data)
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

void insert_at(LinkedList* list, void* data, int index)
{
    if (index < 0 || index > list->size)
    {
        printf("Index out of bounds\n");
        exit(1);
    }
    LinkedListNode* node = new_linked_list_node(data);
    if (index == 0)
    {
        node->next = list->head;
        list->head = node;
    }
    else
    {
        LinkedListNode* current = list->head;
        for (int i = 0; i < index - 1; i++)
        {
            current = current->next;
        }
        node->next = current->next;
        current->next = node;
    }
    list->size++;
}

void insert(LinkedList* list, void* data)
{
    insert_at(list, data, list->size);
}

void insert_head(LinkedList* list, void* data)
{
    insert_at(list, data, 0);
}

void* remove_at(LinkedList* list, int index)
{
    if (index < 0 || index >= list->size)
    {
        return NULL;
    }
    LinkedListNode* current = list->head;
    LinkedListNode* previous = NULL;
    for (int i = 0; i < index; i++)
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

void* lookup(LinkedList* list, int index)
{
    if (index < 0 || index >= list->size)
    {
        return NULL;
    }
    LinkedListNode* current = list->head;
    for (int i = 0; i < index; i++)
    {
        current = current->next;
    }
    return current->data;
}

void print_list(LinkedList* list, void (*print)(void*))
{
    LinkedListNode* current = list->head;
    printf("[");
    while (current != NULL)
    {
        print(current->data);
        if (current->next != NULL) printf(", ");
        current = current->next;
    }
    printf("]\n");
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

