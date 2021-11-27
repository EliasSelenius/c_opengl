#pragma once

#include <GL.h>
#include "../types.h"

u32 bufferCreate(void* data, u32 size);
void bufferInit(u32 buffer, void* data, u32 size);

typedef enum Filter {
    Filter_Nearest = 9728,
    Filter_Linear = 9729
} Filter;

typedef enum WrapMode {
    WrapMode_Repeat = 10497,
    WrapMode_ClampToBorder = 33069,
    WrapMode_ClampToEdge = 33071,
    WrapMode_MirroredRepeat = 33648
} WrapMode;

u32 texture2DCreate(u32 width, u32 height, b8 genMipmap, Filter filter, WrapMode wrapmode, GLint internalFormat, GLenum format, GLenum type);