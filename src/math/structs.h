#pragma once

#include "../prelude.h"


typedef union vec2 {
    struct { f32 x, y; };
    f32 comps[2];
} vec2;
STATIC_ASSERT(sizeof(vec2) == 8, "Expected vec2 to be 8 bytes.");


typedef union vec3 {
    struct { f32 x, y, z; };
    f32 comps[3];
} vec3;
STATIC_ASSERT(sizeof(vec3) == 12, "Expected vec3 to be 12 bytes.");


typedef union vec4 {
    struct { f32 x, y, z, w; };
    struct { vec3 xyz; }; // Where does this align?
    struct { vec2 xy; vec2 zw; };
    f32 comps[4];
} vec4;
STATIC_ASSERT(sizeof(vec4) == 16, "Expected vec4 to be 16 bytes.");


// A quaternion in the form: w + xi + yj + zk
typedef union quat {
    struct { f32 x, y, z, w; };
    vec3 xyz;
} quat;


typedef union mat4 {
    // NOTE: m[row][col]
    f32 m[4][4];
    struct { vec4 row1, row2, row3, row4; };
    struct { 
        vec3 right;     f32 right_w;
        vec3 up;        f32 up_w;
        vec3 forward;   f32 forward_w;
        vec3 pos;       f32 pos_w;
    };
} mat4;

STATIC_ASSERT(sizeof(mat4) == sizeof(f32) * 16, "Unexpected byte size of mat4.");