#pragma once 

#include "Mesh.h"

typedef struct VertexGroup {
    char* materialName;
    u32 start;
    u32 count;
} VertexGroup;

typedef struct {
    u32 pos_index, normal_index, uv_index;
} vertex_index;

typedef struct {
    vertex_index vertices[3];
} face;

typedef struct MTL {
    char* name;
    vec3 Ka, Kd, Ks, Ke;
    struct MTL* next;
} MTL;

typedef struct OBJ {
    char* name;
    vec3* vertex_positions;
    vec3* vertex_normals;
    face* faces;
    VertexGroup* groups;
    vec3 position;
    vec3 boundingbox[2];

    MTL* mtllib;
    struct OBJ* child;
    struct OBJ* next;
} OBJ;

// typedef struct newOBJ {
//     vec3* vertex_positions;
//     vec3* vertex_normals;
//     face* triangles;

//     struct {
//         char* name;
//         u32 start_tri, count_tri;
//     }* objs;

// } newOBJ;


typedef struct MeshData MeshData;


OBJ* objLoad(const char* filename);
void objFree(OBJ* obj);

void objToFlatShadedMesh(OBJ* obj, MeshData* out_data);
void objToSmoothShadedMesh(OBJ* obj, MeshData* out_data);
void objToMesh(OBJ* obj, u32 normal_threshold_angle, MeshData* out_data);