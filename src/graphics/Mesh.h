#pragma once

#include "../types.h"
#include "../math/vec.h"
#include "obj.h"

typedef struct VertexGroup VertexGroup;
typedef struct MTL MTL;

typedef struct Mesh {
    u32 vao, vbo, ebo;
    // u32 elementCount;
    // VertexGroup* groups;
    u32 drawCount;
    i32* groups_count;
    u64* groups_start; 

    u32 materialsBuffer;
} Mesh;

typedef struct {
    vec3 pos;
    vec3 normal;
} vertex;

typedef struct MeshData {
    u32 vertexCount, indexCount;
    vertex* vertices;
    u32* indices;

    MTL* materialLibrary;
    VertexGroup* groups;
} MeshData;

void meshCreate(u32 vertexCount, vertex* vertices, u32 indexCount, u32* indices, Mesh* out_mesh);
void meshFromData(MeshData* data, Mesh* out_mesh);
void meshRender(Mesh* mesh);
void meshDelete(Mesh* mesh);

void meshGenNormals(MeshData* mesh);
void meshFlipIndices(MeshData* mesh);

void genCube(MeshData* out_result);
void genPlane(MeshData* out_result, u32 res);
void genSphere(MeshData* out_result);