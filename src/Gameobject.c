#include "Gameobject.h"

void gameobjectInit(Mesh* mesh, Gameobject* out_result) {
    out_result->mesh = mesh;
    transformSetDefaults(&out_result->transform);
}

void gameobjectRender(Gameobject* gameobject) {
    mat4 model;
    transformToMatrix(&gameobject->transform, &model);
    bufferInit(app.modelUBO->bufferId, &model, sizeof(mat4));
    meshRender(gameobject->mesh);
}