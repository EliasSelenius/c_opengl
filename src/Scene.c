#include "Scene.h"

#include "types.h"

void sceneInit(Scene* scene) {
    scene->gameobjects = listCreate(Gameobject);
}

void sceneRender(Scene* scene) {
    u32 len = listLength(scene->gameobjects);
    for (int i = 0; i < len; i++) {
        gameobjectRender(&scene->gameobjects[i]);
    }
}