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

void transformPush();
void transformPop();