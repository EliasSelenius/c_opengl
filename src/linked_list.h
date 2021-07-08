#pragma once

#include "types.h"

typedef struct linked_list_node {
    void* item;
    struct linked_list_node* next;
} linked_list_node;

typedef struct {
    u32 count;
    linked_list_node* first;
    linked_list_node* last;
} linked_list;


linked_list* linked_list_ctor();

void linked_list_add(linked_list* list, void* item);
void linked_list_insert(linked_list* list, void* item, u32 index);
void* linked_list_get(linked_list* list, u32 index);

