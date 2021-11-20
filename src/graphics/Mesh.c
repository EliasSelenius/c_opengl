#include "Mesh.h"
#include <GL.h>
#include "glUtils.h"
#include <stdlib.h>

void setupAttribs() {
    vertex* vert = NULL;

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(vertex), &vert->pos);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, false, sizeof(vertex), &vert->color);
}


void meshCreate(u32 vertexCount, vertex* vertices, u32 indexCount, u32* indices, Mesh* out_mesh) {

    out_mesh->vbo = bufferCreate(vertices, sizeof(vertex) * vertexCount);
    out_mesh->ebo = bufferCreate(indices, sizeof(u32) * indexCount);

    out_mesh->elementCount = indexCount;

    glGenVertexArrays(1, &out_mesh->vao);
    glBindVertexArray(out_mesh->vao);

    glBindBuffer(GL_ARRAY_BUFFER, out_mesh->vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, out_mesh->ebo);

    setupAttribs();

    glBindVertexArray(0);
}


void meshRender(Mesh* mesh) {
    glBindVertexArray(mesh->vao);
    glDrawElements(GL_TRIANGLES, mesh->elementCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}


void meshDelete(Mesh* mesh) {
    // delete vbo and ebo
    glDeleteBuffers(2, &mesh->vbo);

    glDeleteVertexArrays(1, &mesh->ebo);

    mesh->vao = mesh->vbo = mesh->ebo = 0;
}



void meshGenNormals(MeshData* mesh) {
    // set all normals to zero
    for (int i = 0; i < mesh->vertexCount; i++) {
        mesh->vertices[i].normal = (vec3){ 0, 0, 0 };
    }

    // add every triangles contribution to every vertex normal
    for (int i = 0; i < mesh->indexCount; i += 3) {
        u32 i0 = mesh->indices[i],
            i1 = mesh->indices[i + 1],
            i2 = mesh->indices[i + 2];

        vec3 normal;
        // TODO: calc normal. vector cross prod.

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

    u32 vi = 0, i = 0;
    for (int x = 0; x <= res; x++) {
        for (int y = 0; y <= res; y++) {
            out_result->vertices[x * vres + y] = (vertex){
                .pos = { x - half_res, 0, y - half_res },
                .color = { 1, 1, 1, 1 }
            };

            if (x < res && y < res) {
                out_result->indices[i++] = vi;
                out_result->indices[i++] = vi + 1;
                out_result->indices[i++] = vi + vres + 1;

                out_result->indices[i++] = vi;
                out_result->indices[i++] = vi + vres;
                out_result->indices[i++] = vi + vres + 1;
            }

            vi++;
        }
    } 
}

void genSphere(MeshData* out_result);