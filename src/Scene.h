#pragma once

#include "Gameobject.h"
#include "List.h"

typedef struct Scene {
    Gameobject* gameobjects;
} Scene;

void sceneInit(Scene* scene);
void sceneRender(Scene* scene); 