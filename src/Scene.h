#pragma once

#include "Gameobject.h"
#include "List.h"

typedef struct Scene {
    Gameobject* gameobjects;
} Scene;

// TODO: replace with sceneCreate() 
void sceneInit(Scene* scene);
void sceneRender(Scene* scene);
u32 sceneAddObject(Scene* scene, Mesh* mesh);