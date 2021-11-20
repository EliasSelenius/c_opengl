#pragma once
#include "vec.h"
#include "quat.h"

typedef struct Transform {
    vec3 position, scale;
    quat rotation;
} Transform;

void transformSetDefaults(Transform* transform);

void transformToMatrix(Transform* transform, mat4* out_result);

void transformTranslate(Transform* transform, vec3 translation);
void transformRotate(Transform* transform, quat rotation);
void transformRotateAxisAngle(Transform* transform, vec3 axis, f32 angle);