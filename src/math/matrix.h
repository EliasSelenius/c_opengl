#pragma once

#include "../types.h"
#include "vec.h"

typedef union {
    f32 m[4][4];
    struct {
        vec4 row1, row2, row3, row4;
    };
} mat4;


void mat4Mul(mat4* left, mat4* right, mat4* out_result);
void mat4SetIdentity(mat4* m);


// NOTE: perspective functions derived from OpenTK
void mat4Perspective(f32 fovy, f32 aspect, f32 depthNear, f32 depthFar, mat4* out_result);
void mat4PerspectiveOffCenter(f32 left, f32 right, f32 bottom, f32 top, f32 depthNear, f32 depthFar, mat4* out_result);

void transformPush();
void transformPop();