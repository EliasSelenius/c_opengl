#pragma once

#include "../types.h"

typedef union {
    struct { f32 x, y; };
    f32 comps[2];
} vec2;

STATIC_ASSERT(sizeof(vec2) == 8, "Expected vec2 to be 8 bytes.");


typedef union {
    struct { f32 x, y, z; };
    f32 comps[3];
} vec3;

STATIC_ASSERT(sizeof(vec3) == 12, "Expected vec3 to be 12 bytes.");


typedef union {
    struct { f32 x, y, z, w; };
    f32 comps[4];
} vec4;

STATIC_ASSERT(sizeof(vec4) == 16, "Expected vec4 to be 16 bytes.");

f32 vec4dot(vec4* a, vec4* b);

//f32 vec2Length(vec2 v) { return v.x*v.x + v.y*v.y; }
//f32 vec3Length(vec3 v) { return v.x*v.x + v.y*v.y + v.z*v.z; }
//f32 vec4Length(vec4 v) { return v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w; }

//vec2 vec2(f32 x, f32 y) { return {}}