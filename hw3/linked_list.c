#include "linked_list.h"

static void free_ll_node(LinkedList* ll, LinkedListNode* node);
static LinkedListNode* lln_create(LinkedList* ll, void* value);


/**
 * Create empty linked list.
 */
LinkedList* ll_create(void (*free_func)(void*), void* (*copy_func)(void*)){
    LinkedList* ll = (LinkedList*)malloc(sizeof(LinkedList));
    if (!ll){
        perror("malloc failed.");
        return NULL;
    }
    ll->size = 0;
    ll->head = NULL;
    ll->free_func = free_func;
    ll->copy_func = copy_func;
    return ll;
}


/**
 * Free a linked list node and its value with a provided free_func.
 */
static void free_ll_node(LinkedList* ll, LinkedListNode* node){
    ll->free_func(node->value);
    free(node);
}


/**
 * Free a linked list and its elements.
 * Need to free the list itself, each of its nodes, and the values associated with each node.
 */
void ll_free(LinkedList* ll){
    LinkedListNode* current = ll->head;
    while (current){
        LinkedListNode* next = current->next;
        free_ll_node(ll, current);
        current = next;
    }
    ll->size = 0;
    free(ll);
}


static LinkedListNode* lln_create(LinkedList* ll, void* value){
    LinkedListNode* lln = (LinkedListNode*)malloc(sizeof(LinkedListNode));
    if (!lln){
        perror("malloc failed.");
        return NULL;
    }
    lln->value = ll->copy_func(value);
    return lln;
}


/**
 * Prepend element to linked list.
 */
void ll_prepend(LinkedList* ll, void* elem){
    LinkedListNode* new_node = lln_create(ll, elem);
    LinkedListNode* old_head = ll->head;
    new_node->next = old_head;
    ll->head = new_node;
    ll->size++;
}

/**
 * Get a reference to an element in the linked list.
 */
void* ll_get(LinkedList* ll, size_t idx){
    if (idx >= ll->size){
        return NULL;
    }

    LinkedListNode* current = ll->head;
    for (int i = 0; i < idx; i++){
        current = current->next;
    }
    return current->value;
}


/**
 * Remove an element from the linked list/
 */
void ll_remove(LinkedList* ll, size_t idx){
    if (idx >= ll->size){
        return;
    }
    
    LinkedListNode* current = ll->head;
    LinkedListNode* last = NULL;
    for (int i = 0; i < idx; i++){
        last = current;
        current = current->next;
    }

    if (!last){
        // Removing first element in list
        ll->head = current->next;
    }
    else {
        last->next = current->next;
    }
    free_ll_node(ll, current);
    ll->size--;
}
