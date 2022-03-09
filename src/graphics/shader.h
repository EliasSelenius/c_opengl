#pragma once

#include "../types.h"
#include "../String.h"

typedef struct Ublock {
    char* name;
    u32 bindingPoint;

    u32 bufferId;
} Ublock;

/*
typedef struct ShaderProgram {
    char* name;
    u32 id;
    time_t timeCreated;

} ShaderProgram;
*/



u32 shaderCreate(const char* vert, const char* frag, const char* geom);
u32 shaderCreateCompute(const char* src);

u32 shaderLoadCompute(const char* name);
u32 shaderLoadByName(const char* name);
u32 shaderLoad(const char* frag_filename, const char* vert_filename, const char* geom_filename);
void shaderReload(char* name, u32 program);

Ublock* ublockGetByName(char* name);
void ublockBindBuffer(Ublock* ublock, u32 buffer);