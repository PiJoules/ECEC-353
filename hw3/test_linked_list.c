#include "chat.h"

/**
 * Test the linked list functions.
 * Run valgrind on this.
 */

int main(int argc, char* argv[]){
    List* list = create_list();

    assert(list->size == 0 && list->head == NULL);

    append_to_list(list, "elem1");
    append_to_list(list, "elem2");
    append_to_list(list, "elem3");

    assert(list->size == 3);
    assert(!strcmp(item_at(list, 0), "elem1"));
    assert(!strcmp(item_at(list, 1), "elem2"));
    assert(!strcmp(item_at(list, 2), "elem3"));

    assert(value_in_list(list, "elem1"));
    assert(!value_in_list(list, "elem4"));

    remove_from_list(list, 1);
    assert(list->size == 2);
    assert(!strcmp(item_at(list, 0), "elem1"));
    assert(!strcmp(item_at(list, 1), "elem3"));

    free_list(list);

    return 0;
}
