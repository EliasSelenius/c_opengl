#include "Transform.h"

void transformSetDefaults(Transform* transform) {
    transform->position = (vec3){ 0, 0, 0 };
    transform->scale = (vec3){ 1, 1, 1 };
    quatSetIdentity(&transform->rotation);
}

void transformToMatrix(Transform* transform, mat4* out_result) {   
    quatToMatrix(&transform->rotation, out_result);

    // TODO: implement scale
    
    out_result->row4.xyz = transform->position;
}

void transformTranslate(Transform* transform, vec3 translation);
void transformRotate(Transform* transform, quat rotation);
void transformRotateAxisAngle(Transform* transform, vec3 axis, f32 angle);