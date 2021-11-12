#pragma once

#include <GL.h>
#include "../types.h"

u32 bufferCreate(void* data, u32 size);
void bufferInit(u32 buffer, void* data, u32 size);
