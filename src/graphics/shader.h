#pragma once

#include "../types.h"


u32 shaderCreate(const char* vert, i32 vertLength, const char* frag, i32 fragLength, const char* geom, i32 geomLength);
u32 shaderLoad(const char* name);