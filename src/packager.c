#include "prelude.h"
#include "math/Transform.h"
#include "graphics/Mesh.h"
#include "graphics/obj.h"
#include "List.h"
#include "Application.h"
#include "GL.h"

#include <stdio.h>
#include <stdlib.h>

#include "packager.h"

#define PACKAGE_NAME "package.bin"


/*
    Package data layout
        - list of models
        - list of materials
        - list of vertices
        - list of indices
*/

Package package;

void packageCreate() {

    OBJ* obj = objLoad("src/models/Ships.obj");
    
    u32 modelsCount = 1;
    u32 materialsCount = 0;
    u32 verticesCount = 0;
    u32 indicesCount = 0;

    { // count models buffer size
        OBJ* next = obj;
        while ((next = next->next)) modelsCount++;
    }
    Model* models = malloc(sizeof(Model) * modelsCount);

    { // fill model buffer
        u32 modelIndex = 0;

        OBJ* next = obj;
        while (next) {
            Model* model = &models[modelIndex++];

            u32 vLen = listLength(next->vertex_positions);
            u32 iLen = listLength(next->faces) * 3;

            *model = (Model) {

                .transform.position = obj->position,
                .transform.rotation = (quat) {0, 0, 0, 1},

                .parent_index = -1,

                .vertices_start = verticesCount,
                .vertices_count = vLen,
                .tris.material_index = 0,
                .tris.indices_start = indicesCount,
                .tris.indices_count = iLen
            };

            verticesCount += vLen;
            indicesCount += iLen;

            next = next->next;
        }
    }

    Material* materials;

    { // materials

        // count materials
        MTL* mtl = obj->mtllib;
        while (mtl) {
            materialsCount++;
            mtl = mtl->next;
        }

        materials = malloc(sizeof(Material) * materialsCount);

        // fill materials buffer
        u32 mIndex = 0;
        mtl = obj->mtllib;
        while (mtl) {
            materials[mIndex++] = (Material) {
                .albedo = mtl->Kd,
                .roughness = 0.1f,
                .metallic = 0.1f
            };
            
            mtl = mtl->next;
        }
    }


    vertex* vertices = malloc(sizeof(vertex) * verticesCount);
    u32* indices = malloc(sizeof(u32) * indicesCount);

    { // fill buffers

        u32 vIndex = 0;
        u32 iIndex = 0;

        OBJ* next = obj;
        while (next) {

            u32 vLen = listLength(next->vertex_positions);
            for (u32 i = 0; i < vLen; i++) {
                vertices[vIndex++] = (vertex) {
                    .pos = next->vertex_positions[i],
                    .normal = (vec3) {0, 1, 0} // TODO: gen normals
                };
            }

            u32 fLen = listLength(next->faces);
            for (u32 i = 0; i < fLen; i++) {
                indices[iIndex++] = next->faces[i].vertices[0].pos_index;
                indices[iIndex++] = next->faces[i].vertices[1].pos_index;
                indices[iIndex++] = next->faces[i].vertices[2].pos_index;
            }

            next = next->next;
        }
    }

    { // node graph for models buffer
        objApproximateNodeGraph(obj);
        
        u32 index = 0;

        OBJ* next = obj;
        while (next) {
            u32 parentModelIndex = index++;

            OBJ* child = next->child;            
            while (child) {
                models[index++].parent_index = parentModelIndex;
                child = child->next;
            }

            next = next->next;
        }
    }


    objFree(obj);

    { // write to file
        FILE* file;
        if (fopen_s(&file, PACKAGE_NAME, "wb")) {
            printf("Failed to create package.\n");
            return;
        }

        // models
        fwrite(&modelsCount, sizeof(modelsCount), 1, file);
        fwrite(models, sizeof(Model), modelsCount, file);

        // materials
        fwrite(&materialsCount, sizeof(materialsCount), 1, file);
        fwrite(materials, sizeof(Material), materialsCount, file);

        // vertices
        fwrite(&verticesCount, sizeof(u32), 1, file);
        fwrite(vertices, sizeof(vertex), verticesCount, file);
        
        // indices
        fwrite(&indicesCount, sizeof(u32), 1, file);
        fwrite(indices, sizeof(u32), indicesCount, file);

        fclose(file);
    }


    free(models);
    free(materials);
    free(vertices);
    free(indices);

}

void packageLoad() {
    FILE* file;
    if (fopen_s(&file, PACKAGE_NAME, "rb")) {
        printf("Failed to open \"%s\".\n", PACKAGE_NAME);
        return;
    }

    // models
    u32 modelsCount = 0;
    fread(&modelsCount, sizeof(modelsCount), 1, file);
    Model* models = malloc(sizeof(Model) * modelsCount);
    fread(models, sizeof(Model), modelsCount, file);

    // materials
    u32 materialsCount = 0;
    fread(&materialsCount, sizeof(materialsCount), 1, file);
    Material* materials = malloc(sizeof(Material) * materialsCount);
    fread(materials, sizeof(Material), materialsCount, file);

    // vertices
    u32 verticesCount = 0;
    fread(&verticesCount, sizeof(verticesCount), 1, file);
    vertex* vertices = malloc(sizeof(vertex) * verticesCount);
    fread(vertices, sizeof(vertex), verticesCount, file);

    // indices
    u32 indicesCount = 0;
    fread(&indicesCount, sizeof(indicesCount), 1, file);
    u32* indices = malloc(sizeof(u32) * indicesCount);
    fread(indices, sizeof(u32), indicesCount, file);

    package = (Package) {
        .modelsCount = modelsCount,
        .models = models,

        .materialsCount = materialsCount,
        .materials = materials,

        .verticesCount = verticesCount,
        .vertices = vertices,

        .indicesCount = indicesCount,
        .indices = indices
    };

    glGenVertexArrays(1, &package.vao);
    glBindVertexArray(package.vao);

    glGenBuffers(1, &package.vbo);
    glGenBuffers(1, &package.ebo);

    glBindBuffer(GL_ARRAY_BUFFER, package.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * package.verticesCount, package.vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, package.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * package.indicesCount, package.indices, GL_STATIC_DRAW);

    vertex* vert = null;
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(vertex), &vert->pos);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(vertex), &vert->normal);

    glBindVertexArray(0);
}