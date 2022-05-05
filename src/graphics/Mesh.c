#include "Mesh.h"
#include <GL.h>
#include "glUtils.h"
#include <stdlib.h>
#include "../List.h"
#include "../Application.h"

void setupAttribs() {
    vertex* vert = NULL;

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(vertex), &vert->pos);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(vertex), &vert->normal);
}


void meshCreate(u32 vertexCount, vertex* vertices, u32 indexCount, u32* indices, Mesh* out_mesh) {

    out_mesh->materialsBuffer = 0;
    out_mesh->groups_count = null;
    out_mesh->groups_start = null;
    out_mesh->drawCount = indexCount;

    out_mesh->vbo = bufferCreate(vertices, sizeof(vertex) * vertexCount);
    out_mesh->ebo = bufferCreate(indices, sizeof(u32) * indexCount);

    // out_mesh->elementCount = indexCount;

    glGenVertexArrays(1, &out_mesh->vao);
    glBindVertexArray(out_mesh->vao);

    glBindBuffer(GL_ARRAY_BUFFER, out_mesh->vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, out_mesh->ebo);

    setupAttribs();

    glBindVertexArray(0);
}

void meshFromData(MeshData* data, Mesh* out_mesh) {

    meshCreate(data->vertexCount, data->vertices, data->indexCount, data->indices, out_mesh);

    if (data->groups) {
        u32 vertGroups = listLength(data->groups);
        if (vertGroups == 0) return;
        out_mesh->drawCount = vertGroups;

        out_mesh->groups_count = malloc(sizeof(u32) * vertGroups);
        out_mesh->groups_start = malloc(sizeof(u64) * vertGroups);

        u32 materialDataSize = sizeof(vec4) * vertGroups;
        vec4* materialData = malloc(materialDataSize);

        for (u32 i = 0; i < vertGroups; i++) {
            VertexGroup g = data->groups[i];
            out_mesh->groups_count[i] = g.count;
            out_mesh->groups_start[i] = (g.start * sizeof(u32));

            MTL* mtl = data->materialLibrary;
            while (mtl && !stringStartsWith(mtl->name, g.materialName)) mtl = mtl->next;
            if (mtl) {
                materialData[i] = (vec4) { mtl->Kd.x, mtl->Kd.y, mtl->Kd.z, 1.0 };
            }
        }

        out_mesh->materialsBuffer = bufferCreate(materialData, materialDataSize);
        free(materialData);

    }
}

void meshRender(Mesh* mesh) {
    glBindVertexArray(mesh->vao);


    uboBindBuffer(uboGetByName("Material"), mesh->materialsBuffer);

    if (mesh->groups_count) {
        glMultiDrawElements(GL_TRIANGLES, mesh->groups_count, GL_UNSIGNED_INT, mesh->groups_start, mesh->drawCount);   
    } else {
        glDrawElements(GL_TRIANGLES, mesh->drawCount, GL_UNSIGNED_INT, 0);
    }

    glBindVertexArray(0);
}


void meshDelete(Mesh* mesh) {
    // delete vbo and ebo
    glDeleteBuffers(2, &mesh->vbo);

    glDeleteVertexArrays(1, &mesh->ebo);

    free(mesh->groups_count);
    free(mesh->groups_start);

    mesh->vao = mesh->vbo = mesh->ebo = 0;
}



void meshGenNormals(MeshData* mesh) {
    // set all normals to zero
    for (int i = 0; i < mesh->vertexCount; i++) {
        mesh->vertices[i].normal = (vec3){ 0, 0, 0 };
    }

    //return;

    // add every triangles contribution to every vertex normal
    for (int i = 0; i < mesh->indexCount; i += 3) {
        u32 i0 = mesh->indices[i],
            i1 = mesh->indices[i + 1],
            i2 = mesh->indices[i + 2];


        vec3 v0 = mesh->vertices[i0].pos;
        vec3 v1 = mesh->vertices[i1].pos;
        vec3 v2 = mesh->vertices[i2].pos;
        vec3Sub(&v0, v2);
        vec3Sub(&v1, v2);

        vec3 normal;
        vec3Cross(&v0, &v1, &normal);


        vec3Add(&mesh->vertices[i0].normal, normal);
        vec3Add(&mesh->vertices[i1].normal, normal);
        vec3Add(&mesh->vertices[i2].normal, normal);
    }

    // normalize vertex normals
    for (int i = 0; i < mesh->vertexCount; i++) {
        vec3Normalize(&mesh->vertices[i].normal);
    }
}

void meshFlipIndices(MeshData* mesh) {
    for (int i = 0; i < mesh->indexCount; i += 3) {
        u32 t = mesh->indices[i];
        mesh->indices[i] = mesh->indices[i + 2];
        mesh->indices[i + 2] = t;
    }
}




void genCube(MeshData* out_result) {
    out_result->vertices = (vertex[]) {
        {
            .pos = { 1, 2, 3 }
        },
        {
            .pos = { 1, 2, 3 }
        }
    };
}

/*

    res = 4
        *---*---*---*---*
        |   |   |   |   |
        *---*---*---*---*
        |   |   |   |   |
        *---*---*---*---*
        |   |   |   |   |
        *---*---*---*---*
        |   |   |   |   |
        *---*---*---*---*

*/
void genPlane(MeshData* out_result, u32 res) {

    u32 vres = res + 1;
    f32 half_res = (f32)res * 0.5f;

    u32 vcount = out_result->vertexCount = vres * vres;
    u32 icount = out_result->indexCount = res * res * 2 * 3; 

    out_result->vertices = malloc(sizeof(vertex) * vcount);
    out_result->indices = malloc(sizeof(u32) * icount);
    out_result->groups = null;

    u32 vi = 0, i = 0;
    for (int x = 0; x <= res; x++) {
        for (int y = 0; y <= res; y++) {
            out_result->vertices[x * vres + y] = (vertex){
                .pos = { x - half_res, 0, y - half_res }
            };

            if (x < res && y < res) {
                out_result->indices[i++] = vi;
                out_result->indices[i++] = vi + 1;
                out_result->indices[i++] = vi + vres + 1;

                out_result->indices[i++] = vi;
                out_result->indices[i++] = vi + vres + 1;
                out_result->indices[i++] = vi + vres;
            }

            vi++;
        }
    }

    meshGenNormals(out_result);
}

void genSphere(MeshData* out_result);