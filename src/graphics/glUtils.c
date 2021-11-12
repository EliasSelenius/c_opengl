#include "glUtils.h"


u32 bufferCreate(void* data, u32 size) {
    u32 b;
    glGenBuffers(1, &b);
    bufferInit(b, data, size);
    return b;
}

void bufferInit(u32 buffer, void* data, u32 size) {
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}