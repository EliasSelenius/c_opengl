#pragma once

#include "../types.h"
#include "../String.h"


u32 shaderCreate(const char* vert, i32 vertLength, const char* frag, i32 fragLength, const char* geom, i32 geomLength);
u32 shaderCreateCompute(const char* src);

u32 shaderLoadCompute(const char* name);
u32 shaderLoadByName(const char* name);
u32 shaderLoad(const char* frag_filename, const char* vert_filename, const char* geom_filename);

void shaderLoadSource(StringBuilder* sb, const char* filename);