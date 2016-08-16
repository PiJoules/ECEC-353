#include "chat.h"

List* create_list(){
    SAFE_MALLOC(list, List*, sizeof(List))
    list->size = 0;
    list->head = NULL;
    return list;
}


void free_list(List* list){
    while (list->size){
        remove_from_list(list, 0);
    }
    free(list);
}


/**
 * Append to the end of a list.
 */
void append_to_list(List* list, char* value){
    ListNode* node = (ListNode*)malloc(sizeof(ListNode));
    if (!node){
        fprintf(stderr, "Ran out of space for mallocing linked list node.\n");
        exit(EXIT_FAILURE);
    }

    // Copy the string over instead of pointing to it
    size_t len = strlen(value);
    SAFE_MALLOC(new_str, char*, sizeof(char) * len + 1)
    strncpy(new_str, value, len);
    new_str[len] = '\0';
    node->value = new_str;

    node->next = NULL;
    if (!list->head){
        // Initially empty
        list->head = node;
    }
    else {
        // Has at least 1 value
        ListNode* current = list->head;

        // Move to last value
        while (current->next){
            current = current->next;
        }
        current->next = node;
    }
    list->size++;
}

int value_in_list(List* list, char* value){
    ListNode* current = list->head;
    while (current){
        if (!strcmp(value, current->value)){
            // Same value
            return 1;
        }
        current = current->next;
    }
    return 0;
}

void remove_from_list(List* list, int idx){
    if (idx >= list->size){
        return;
    }

    ListNode* current = list->head;
    ListNode* previous_node = NULL;
    for (int i = 0; i < idx; i++){
        previous_node = current;
        current = current->next;
    }

    if (previous_node){
        previous_node->next = current->next;
    }
    else {
        // Removed the first element
        list->head = current->next;
    }
    free(current->value);
    free(current);
    list->size--;
}


char* item_at(List* list, int idx){
    if (idx >= list->size){
        return NULL;
    }

    ListNode* current = list->head;
    for (int i = 0; i < idx; i++){
        current = current->next;
    }
    return current->value;
}
