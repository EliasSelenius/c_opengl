#pragma once

#include "types.h"
#include "linked_list.h"
#include <stdarg.h>

typedef struct {
    float x, y, z;
} vec3;

typedef struct {
    vec3 pos, scale, rot;
} Transform;

typedef void (*updateFunc)(void*);

typedef struct {
    u32 datasize;
    void* data;
    updateFunc update;
} component;

typedef struct entity {
    Transform transform;
    u32 count;
    component* comps;
} entity;

typedef struct {
    linked_list* entities;
} scene;

entity* create_entity(Transform* transform, const u32 count, ...);
void update_entity(entity* ent);
void destroy_entity(entity* ent);
void enter_scene(scene* scene, entity* entity);


scene* create_scene();
static scene* active_scene;
void set_active_scene(scene* scene);
void update_scene();
