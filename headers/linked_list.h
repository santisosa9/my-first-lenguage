#pragma once
#include <stdbool.h>
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

typedef struct LinkedListIterator
{
    LinkedListNode* current;
} LinkedListIterator;

/* Returns a new emprty LinkedList. */
LinkedList* new_linked_list();

/* Insert new data in the list. */
void insert_ll(LinkedList* list, void* data);

/* Insert new data in the list at the given index. */
void insert_at(LinkedList* list, void* data, nat index);

/* Insert new data at the head of the list. */
void insert_head(LinkedList* list, void* data);

/* Remove the data at the head of the list and return it. */
void* remove_head(LinkedList* list);

/* Remove the data at the head of the list and return it. */
void* remove_tail(LinkedList* list);

/* Remove the data with the given index from the list and retrun it. */
void* remove_at(LinkedList* list, nat index);

/* Search for the data with the given index in the list.
   Returns a pointer to the data if was found, or NULL if not.
*/
void* lookup(LinkedList* list, nat index);

/* Print the list. */
void print_list(LinkedList* list, FILE* f, void (*print)(void*));

/* Free the list. */
void free_list(LinkedList* list);

/* Returns a new LinkedListIterator. */
LinkedListIterator* new_linked_list_iterator(LinkedList* list);

/* Returns the next data in the list. */
void* next(LinkedListIterator* iter);

/* Returns true if there is a next element in the list. */
bool has_next(LinkedListIterator* iter);

/* Free the iterator. */
void free_linked_list_iter(LinkedListIterator* iter);
