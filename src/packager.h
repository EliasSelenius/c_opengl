#pragma once

#include "math/vec.h"
#include "prelude.h"
#include "math/Transform.h"
#include "graphics/Mesh.h"

typedef struct Material {
    vec3 albedo;
    f32 roughness;
    f32 metallic;
} Material;

typedef struct TriangleGroup {
    u32 material_index;
    u32 indices_start;
    u32 indices_count;
} TriangleGroup;

typedef struct Model {
    Transform transform;
    u32 parent_index;

    u32 vertices_start;
    u32 vertices_count;

    TriangleGroup tris;

} Model;

typedef struct Package {
    Model* models;
    u32 modelsCount;

    Material* materials;
    u32 materialsCount;

    vertex* vertices;
    u32 verticesCount;

    u32* indices;
    u32 indicesCount;

    u32 vao;
    u32 vbo;
    u32 ebo;

} Package;

extern Package package;

void packageCreate();
void packageLoad();

inline void genMeshFromModel(Model* model, Mesh* mesh) {
    meshCreate(
        model->vertices_count,
        package.vertices,
        model->tris.indices_count,
        package.indices, mesh);    
}

inline void packageDraw() {

    // mat4 matrix;
    // mat4SetIdentity(&matrix);
    // matrix.pos = (vec3) { -30, 10, -10 };
    // bufferInit(app.modelUBO->bufferId, &matrix, sizeof(mat4));

    glBindVertexArray(package.vao);
    for (u32 i = 0; i < package.modelsCount; i++) {
        u32 modelIndex = i;
        u32 indCount = package.models[modelIndex].tris.indices_count;
        u64 indStart = (u64)package.models[modelIndex].tris.indices_start;
        u32 basevert = package.models[modelIndex].vertices_start;


        // glDrawElements(GL_TRIANGLES, indCount, GL_UNSIGNED_INT, (const void*)(indStart * sizeof(u32)));
        glDrawElementsBaseVertex(GL_TRIANGLES, indCount, GL_UNSIGNED_INT, (const void*)(indStart * sizeof(u32)), basevert);
    }
    glBindVertexArray(0);
}