#pragma once
#include "utils.h"

typedef struct LinkedListNode
{
    void* data;
    struct LinkedListNode* next;
} LinkedListNode;

typedef struct LinkedList
{
    nat size;
    LinkedListNode* head;
} LinkedList;

/* Returns a new LinkedListNode initialized with data. */
LinkedListNode* new_linked_list_node(void* data);

/* Returns a new emprty LinkedList. */
LinkedList* new_linked_list();

/* Insert new data in the list. */
void insert(LinkedList* list, void* data);

/* Insert new data in the list at the given index. */
void insert_at(LinkedList* list, void* data, int index);

/* Insert new data at the head of the list. */
void insert_head(LinkedList* list, void* data);

/* Remove the data at the head of the list and return it. */
void* remove_head(LinkedList* list);

/* Remove the data at the head of the list and return it. */
void* remove_tail(LinkedList* list);

/* Remove the data with the given index from the list and retrun it. */
void* remove_at(LinkedList* list, int index);

/* Search for the data with the given index in the list.
   Returns a pointer to the data if was found, or NULL if not.
*/
void* lookup(LinkedList* list, int index);

/* Print the list. */
void print_list(LinkedList* list, void (*print)(void*));

/* Free the list. */
void free_list(LinkedList* list);


