#pragma once 

#include "../types.h"

// A quaternion in the form: w + xi + yj + zk
typedef struct quat {
    f32 x, y, z, w;
} quat;

void quatMul(quat* left, quat* right, quat* out_result);

void quatNormalize(quat* q);
void quatSetIdentity(quat* q);

void quatFromAxisAngle(vec3* axis, f32 angle, quat* out_result);
void quatToAxisAngle(quat* q, f32* out_angle, vec3* out_axis);

void quatFromMatrix(mat4* m, quat* out_result);
void quatToMatrix(quat* q, mat4* out_result);

void quatLeft(quat* q, vec3* v);
void quatUp(quat* q, vec3* v);
void quatForward(quat* q, vec3* v);
