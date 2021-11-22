#include "Transform.h"
#include "matrix.h"

void transformSetDefaults(Transform* transform) {
    transform->position = (vec3){ 0, 0, 0 };
    transform->scale = (vec3){ 1, 1, 1 };
    quatSetIdentity(&transform->rotation);
}

void transformToMatrix(Transform* transform, mat4* out_result) {

    quatToMatrix(&transform->rotation, out_result);

    // TODO: implement scale
    
    //out_result->row4.xyz = transform->position;

    out_result->row4.x = transform->position.x;
    out_result->row4.y = transform->position.y;
    out_result->row4.z = transform->position.z;
}

void transformTranslate(Transform* transform, vec3 translation) {
    transform->position.x += translation.x;
    transform->position.y += translation.y;
    transform->position.z += translation.z;
}

void transformRotate(Transform* transform, quat rotation) {
    quatMul(&transform->rotation, &rotation, &transform->rotation);
}

void transformRotateAxisAngle(Transform* transform, vec3 axis, f32 angle) {
    quat q;
    quatFromAxisAngle(&axis, angle, &q);
    transformRotate(transform, q);
}