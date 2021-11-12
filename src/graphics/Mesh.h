#pragma once

#include "../types.h"
#include "../math/vec.h"

typedef struct {
    u32 vao, vbo, ebo;
    u32 elementCount;
} Mesh;

typedef struct {
    vec3 pos;
    vec4 color;
} vertex;

void meshCreate(u32 vertexCount, vertex* vertices, u32 indexCount, u32* indices, Mesh* out_mesh);
void meshRender(Mesh* mesh);
void meshDelete(Mesh* mesh);