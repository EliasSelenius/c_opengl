#include "List.h"

#include <stdlib.h>
#include <string.h>

ListHead* listHead(void* list) { return &((ListHead*)list)[-1]; }

u32 listLength(void* list)   { return ((u32*)list)[-3]; }
u32 listCapacity(void* list) { return ((u32*)list)[-2]; }
u32 listStride(void* list)   { return ((u32*)list)[-1]; }


void* _listCreate(u32 stride) {
    ListHead* head = malloc(
        sizeof(ListHead) +
        stride        // first element
    );

    void* list = &head[1];

    head->capacity = 1;
    head->length = 0;
    head->stride = stride;

    return list;
}

void listDelete(void* list) {
    free(listHead(list));
}

void _listAdd(void** list, void* value) {
    ListHead* head = listHead(*list);

    if (head->length == head->capacity) {
        // resize
        head->capacity *= 2;
        head = realloc(head, sizeof(ListHead) + head->stride * head->capacity);
        *list = &head[1];
    }

    
    u64 addr = (u64)*list;
    addr += head->length * head->stride;
    memcpy((void*)addr, value, head->stride);

    //u32 bytePos = head->length * head->stride;
    //((u8*)list)[bytePos] = *value;

    head->length++;

}

void listRemoveValue(void* list, void* value) {

}

void listClear(void* list) {
    listHead(list)->length = 0;
}