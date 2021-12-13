#pragma once 

#include "Mesh.h"

typedef struct {
    u32 pos_index, normal_index, uv_index;
} vertex_index;

typedef struct {
    vertex_index vertices[3];
} face;

typedef struct OBJ {
    vec3* positions;
    vec3* normals;
    face* faces;
} OBJ;

void objLoad(const char* filename, OBJ* out_obj);
void objFree(OBJ* obj);

void objToFlatShadedMesh(OBJ* obj, MeshData* out_data);
void objToSmoothShadedMesh(OBJ* obj, MeshData* out_data);
void objToMesh(OBJ* obj, u32 normal_threshold_angle, MeshData* out_data);