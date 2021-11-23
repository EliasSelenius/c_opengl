#pragma once
#include "math/Transform.h"
#include "graphics/Mesh.h"
#include "graphics/glUtils.h"
#include "Application.h"

typedef struct Gameobject {
    Transform transform;
    Mesh* mesh;

    struct Gameobject* parent;
} Gameobject;


void gameobjectInit(Mesh* mesh, Gameobject* out_result);
void gameobjectRender(Gameobject* gameobject);