#include "Transform.h"
#include "matrix.h"

void transformSetDefaults(Transform* transform) {
    transform->position = (vec3){ 0, 0, 0 };
    quatSetIdentity(&transform->rotation);
}

void transformToMatrix(Transform* transform, mat4* out_result) {

    quatToMatrix(&transform->rotation, out_result);

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

    quatNormalize(&transform->rotation); // solves the problem of the camera rotation getting totally fucked up. 
    // Other things can probably also happen so its a good thing to have to make sure rotations always stays normalized
}

void transformRotateAxisAngle(Transform* transform, vec3 axis, f32 angle) {
    quat q;
    quatFromAxisAngle(&axis, angle, &q);
    transformRotate(transform, q);
}

void transformLookAt(Transform* transform, vec3 point, vec3 up) {

    mat4 newRot = (mat4) {
        // side vec
        0, 0, 0, 0,
        // up vec
        0, 0, 0, 0,
        // forward vec
        point.x, point.y, point.z, 0,

        0, 0, 0, 0
    };

    // construct forward vector
    vec3Sub(&newRot.row3.xyz, transform->position);
    vec3Normalize(&newRot.row3.xyz);
    
    // construct side vector
    vec3Cross(&up, &newRot.row3.xyz, &newRot.row1.xyz);
    vec3Normalize(&newRot.row1.xyz);

    // construct up vector
    vec3Cross(&newRot.row3.xyz, &newRot.row1.xyz, &newRot.row2.xyz);
    vec3Normalize(&newRot.row2.xyz);
    
    quatFromMatrix(&newRot, &transform->rotation);
}