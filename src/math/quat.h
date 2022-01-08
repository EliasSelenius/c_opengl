#pragma once 

#include "../types.h"
#include "structs.h"

void quatMul(quat* left, quat* right, quat* out_result);

quat conjugate(quat q);
void quatNormalize(quat* q);
void quatSetIdentity(quat* q);
void quatSlerp(quat* a, quat* b, f32 t, quat* out_result);

void quatRotateVec3(quat rot, vec3* v);

void quatFromAxisAngle(vec3* axis, f32 angle, quat* out_result);
void quatToAxisAngle(quat* q, f32* out_angle, vec3* out_axis);

void quatFromMatrix(mat4* m, quat* out_result);
void quatToMatrix(quat* q, mat4* out_result);

void quatLeft(quat* q, vec3* v);
void quatUp(quat* q, vec3* v);
void quatForward(quat* q, vec3* v);

