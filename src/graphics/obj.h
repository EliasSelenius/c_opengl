#pragma once 

#include "Mesh.h"

typedef struct {
    u32 pos_index, normal_index, uv_index;
} vertex_index;

typedef struct {
    vertex_index vertices[3];
} face;

typedef struct OBJ {
    char* name;
    vec3* positions;
    vec3* normals;
    face* faces;
    vec3 pos;
} OBJ;

typedef struct OBJfile {
    OBJ* objs; // darray
} OBJfile;

void objLoad(const char* filename, OBJfile* objFile);
void objFree(OBJfile* obj);

void objToFlatShadedMesh(OBJ* obj, MeshData* out_data);
void objToSmoothShadedMesh(OBJ* obj, MeshData* out_data);
void objToMesh(OBJ* obj, u32 normal_threshold_angle, MeshData* out_data);