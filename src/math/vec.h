#pragma once

#include "../types.h"
#include "structs.h"


void vec3Add(vec3* a, vec3 b);
void vec3Normalize(vec3* v);
void vec3Cross(vec3* a, vec3* b, vec3* out_result);

f32 vec4dot(vec4* a, vec4* b);

//f32 vec2Length(vec2 v) { return v.x*v.x + v.y*v.y; }
//f32 vec3Length(vec3 v) { return v.x*v.x + v.y*v.y + v.z*v.z; }
//f32 vec4Length(vec4 v) { return v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w; }

//vec2 vec2(f32 x, f32 y) { return {}}