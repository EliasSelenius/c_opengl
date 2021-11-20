#pragma once

#include "../types.h"
#include "../math/vec.h"

typedef struct {
    u32 vao, vbo, ebo;
    u32 elementCount;
} Mesh;

typedef struct {
    vec3 pos;
    vec3 normal;
    vec4 color;
} vertex;

STATIC_ASSERT(sizeof(vertex) == 40, "");

typedef struct MeshData {
    u32 vertexCount, indexCount;
    vertex* vertices;
    u32* indices;
} MeshData;

void meshCreate(u32 vertexCount, vertex* vertices, u32 indexCount, u32* indices, Mesh* out_mesh);
void meshRender(Mesh* mesh);
void meshDelete(Mesh* mesh);

void meshGenNormals(MeshData* mesh);
void meshFlipIndices(MeshData* mesh);

void genCube(MeshData* out_result);
void genPlane(MeshData* out_result, u32 res);
void genSphere(MeshData* out_result);