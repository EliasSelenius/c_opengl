#include "linked_list.h"
#include <stdlib.h>

linked_list* linked_list_ctor() {
    linked_list* l = malloc(sizeof(linked_list));
    l->first = 0;
    l->last = 0;
    l->count = 0;
    return l;
}

void linked_list_add(linked_list* list, void* item) {
    linked_list_node* n = malloc(sizeof(linked_list_node));
    n->item = item;
    n->next = 0;

    if (list->first) {
        list->last->next = n;
        list->last = n;
    } else {
        list->first = n;
        list->last = n;
    }

    list->count++;
}


void linked_list_insert(linked_list* list, void* item, u32 index) {

}

void* linked_list_get(linked_list* list, u32 index) {
    if (list->count <= index) {
        return 0; // TODO: do some error?
    }

    linked_list_node* n = list->first;
    for (int i = 0; i < index; i++) {
        n = n->next;
    }

    return n->item;
}
