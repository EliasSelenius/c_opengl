#include "UBO.h"
#include <GL.h>

#include <stdio.h>
#include <string.h>

Ublock ublocks[30];
static u32 ublockCount = 0;

static Ublock* createNew(char* name) {
    Ublock* ub = &(ublocks[ublockCount]);
    ub->bindingPoint = ublockCount;

    ub->name = malloc(strlen(name));
    strcpy(ub->name, name);
    
    ublockCount++;

    return ub;
}

Ublock* ublockGetByName(char* name) {
    for (int i = 0; i < ublockCount; i++) {
        Ublock* ub = &(ublocks[i]);
        if (strcmp(ub->name, name) == 0) return ub;
    }

    return createNew(name);
}

void ublockBindBuffer(Ublock* ublock, u32 buffer) {
    ublock->bufferId = buffer;
    glBindBufferBase(GL_UNIFORM_BUFFER, ublock->bindingPoint, buffer);
}

void ublockPrintAll() {
    printf("Uniform Blocks: total(%d)\n", ublockCount);
    for (int i = 0; i < ublockCount; i++) {
        Ublock* ub = &(ublocks[i]);
        printf("    %d. %s -> %d\n", ub->bindingPoint, ub->name, ub->bufferId);
    }
}