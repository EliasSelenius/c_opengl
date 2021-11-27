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

u32 texture2DCreate(u32 width, u32 height, b8 genMipmap, Filter filter, WrapMode wrapmode, GLint internalFormat, GLenum format, GLenum type) {
    u32 t;
    glGenTextures(1, &t);
    glBindTexture(GL_TEXTURE_2D, t);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapmode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapmode);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, NULL);

    if (genMipmap) glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
    return t;
}