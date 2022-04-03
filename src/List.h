#pragma once
#include "prelude.h"

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

void* listCreateWithStride(u32 stride);
#define listCreate(type) listCreateWithStride(sizeof(type))
void listDelete(void* list);

void listAddInternal(void** list, void* value); 
#define listAdd(list, value) {   \
    typeof(value) unique_var_name_to_not_fuck_things_up = value;     \
    listAddInternal((void**)&list, &unique_var_name_to_not_fuck_things_up); \
}

i32 listIndexOf(void* list, void* value);
void listRemoveIndex(void* list, u32 index);
void listRemoveValue(void* list, void* value);


void listClear(void* list);


u32 listLength(void* list);
u32 listCapacity(void* list);
u32 listStride(void* list);


