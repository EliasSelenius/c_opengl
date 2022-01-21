#pragma once

#include "../types.h"
#include "structs.h"

f32 vec2Dot(vec2 a, vec2 b);
void vec2Normalize(vec2* v);

void vec3Add(vec3* a, vec3 b);
void vec3Sub(vec3* a, vec3 b);
void vec3Scale(vec3* a, f32 scalar);
f32 vec3Dot(vec3* a, vec3* b);
void vec3Normalize(vec3* v);
void vec3Cross(vec3* a, vec3* b, vec3* out_result);
f32 vec3Length(vec3 v);

f32 vec4Dot(vec4* a, vec4* b);

//f32 vec2Length(vec2 v) { return v.x*v.x + v.y*v.y; }
//f32 vec4Length(vec4 v) { return v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w; }

//vec2 vec2(f32 x, f32 y) { return {}}