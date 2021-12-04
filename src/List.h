#pragma once
#include "types.h"

/*
struct ListHead {
    u32 length, capacity;
};

#define List(type) struct { struct ListHead head; type elements[]; }
*/

/*
    list:
        length
        capacity
        stride
        elements[]
*/

typedef struct {
    u32 length, capacity, stride;
} ListHead;

ListHead* listHead(void* list);

void* _listCreate(u32 stride);
#define listCreate(type) _listCreate(sizeof(type))
void listDelete(void* list);

void _listAdd(void** list, void* value); 
#define listAdd(list, value) {   \
    typeof(value) __unique_var_name_to_not_fuck_things_up = value;     \
    _listAdd((void**)&list, &__unique_var_name_to_not_fuck_things_up); \
}


void listRemove(void* list, void* value);

void listClear(void* list);

u32 listLength(void* list);
u32 listCapacity(void* list);
u32 listStride(void* list);


