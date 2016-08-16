#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct LinkedList LinkedList;
typedef struct LinkedListNode LinkedListNode;

struct LinkedListNode {
    void* value;
    LinkedListNode* next;
};

struct LinkedList {
    size_t size;
    void (*free_func)(void*);
    void* (*copy_func)(void*);
    int (*eq_func)(void*, void*);
    LinkedListNode* head;
};

LinkedList* ll_create();
void ll_free(LinkedList*);
void ll_prepend(LinkedList* ll, void* elem);
void* ll_get(LinkedList* ll, size_t idx);
void ll_remove(LinkedList* ll, size_t idx);
LinkedList* ll_copy(LinkedList* ll);
int ll_equal(LinkedList* ll, LinkedList* other);
char* ll_str(LinkedList* ll);

#endif
