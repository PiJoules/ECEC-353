#include "linked_list.h"

static void free_ll_node(LinkedList* ll, LinkedListNode* node);
static LinkedListNode* lln_create(LinkedList* ll, void* value);
static char* str_concat(const char* str1, const char* str2);
static int str_ends_with(const char* str, const char* substr);


/**
 * Create empty linked list.
 */
LinkedList* ll_create(void (*free_func)(void*), void* (*copy_func)(void*), int (*eq_func)(void*, void*)){
    LinkedList* ll = (LinkedList*)malloc(sizeof(LinkedList));
    if (!ll){
        perror("malloc failed.");
        return NULL;
    }
    ll->size = 0;
    ll->head = NULL;
    ll->free_func = free_func;
    ll->copy_func = copy_func;
    ll->eq_func = eq_func;
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

/**
 * Shallow copy a linked list.
 */
LinkedList* ll_copy(LinkedList* ll){
    LinkedList* new_ll = ll_create(ll->free_func, ll->copy_func, ll->eq_func);

    if (!ll->size){
        // Empty
        return new_ll;
    }

    for (int i = ll->size-1; i >= 0; i--){
        ll_prepend(new_ll, ll_get(ll, i));
    }
    return new_ll;
}

/**
 * Compare linked lists.
 */
int ll_equal(LinkedList* ll, LinkedList* other){
    if (ll->size != other->size){
        return 0;
    }

    LinkedListNode* node1 = ll->head;
    LinkedListNode* node2 = other->head;
    while (node1){
        if (!ll->eq_func(node1->value, node2->value)){
            return 0;
        }
        node1 = node1->next;
        node2 = node2->next;
    }
    return 1;
}


static char* str_concat(const char* s1, const char* s2){
    size_t len1 = strlen(s1);
    size_t len2 = strlen(s2);
    char *result = malloc(len1+len2+1);//+1 for the zero-terminator
    if (!result){
        perror("malloc failed.");
        return NULL;
    }
    memcpy(result, s1, len1);
    memcpy(result+len1, s2, len2+1);//+1 to copy the null-terminator
    return result;
}


static int str_ends_with(const char* str, const char* substr){
    if (!strlen(substr)){
        // Ends with nothing
        return 1;
    }
    char* first_inst = strrchr(str, substr[0]);
    return first_inst && !strcmp(first_inst, substr);
}


/**
 * Linked list to string representation.
 * The value MUST BE and IS TREATED AS a string/char*.
 */
char* ll_str(LinkedList* ll){
    char* result = (char*)malloc(sizeof(char) * 2);
    if (!result){
        perror("malloc failed.");
        return NULL;
    }
    result[0] = '[';
    result[1] = '\0';

    LinkedListNode* node = ll->head;
    for (int i = 0; i < ll->size; i++){
        // Add string value
        char* new_result = str_concat(result, (char*)node->value);
        free(result);
        result = new_result;

        // Add separator
        new_result = str_concat(result, ", ");
        free(result);
        result = new_result;

        node = node->next;
    }

    // Remove trailing delimiter
    if (str_ends_with(result, ", ")){
        size_t new_len = strlen(result) - 2;
        char* new_str = (char*)malloc(sizeof(char) * new_len + 1);
        strncpy(new_str, result, new_len);
        new_str[new_len] = '\0';
        free(result);
        result = new_str;
    }

    char* new_result = str_concat(result, "]");
    free(result);
    return new_result;
}

/**
 * Remove from the linked list by value.
 */
void ll_remove_value(LinkedList* ll, void* value){
    LinkedListNode* current = ll->head;
    LinkedListNode* last = NULL;
    while (current){
        if (ll->eq_func(current->value, value)){
            // Found it
            if (!last){
                // Removing first element in list
                ll->head = current->next;
            }
            else {
                last->next = current->next;
            }
            free_ll_node(ll, current);
            ll->size--;
            return;
        }
        last = current;
        current = current->next;
    }
}
