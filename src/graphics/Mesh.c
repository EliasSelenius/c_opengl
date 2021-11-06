#include "Mesh.h"
#include <GL.h>

u32 bufferCreate(void* data, u32 size) {
    u32 b;
    glGenBuffers(1, &b);
    glBindBuffer(GL_ARRAY_BUFFER, b);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return b;
}

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