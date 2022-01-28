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

u32 sceneAddObject(Scene* scene, Mesh* mesh) {
    Gameobject o;
    gameobjectInit(mesh, &o);

    listAdd(scene->gameobjects, o);
    return listLength(scene->gameobjects) - 1;
} 