#pragma once

#include "../prelude.h"
#include "structs.h"

#include <math.h>

f32 vec2Dot(vec2 a, vec2 b);
void vec2Normalize(vec2* v);

void vec3Add(vec3* a, vec3 b);
void vec3Sub(vec3* a, vec3 b);
void vec3Scale(vec3* a, f32 scalar);
f32 vec3Dot(vec3* a, vec3* b);
void vec3Normalize(vec3* v);
void vec3Cross(vec3* a, vec3* b, vec3* out_result);
vec3 cross(vec3 a, vec3 b);
f32 vec3Length(vec3 v);

f32 vec4Dot(vec4* a, vec4* b);

//f32 vec2Length(vec2 v) { return v.x*v.x + v.y*v.y; }
//f32 vec4Length(vec4 v) { return v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w; }

//vec2 vec2(f32 x, f32 y) { return {}}


inline f32 v3dot(vec3 a, vec3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
inline vec3 v3add(vec3 a, vec3 b) { return (vec3) { a.x + b.x, a.y + b.y, a.z + b.z }; }
inline vec3 v3sub(vec3 a, vec3 b) { return (vec3) { a.x - b.x, a.y - b.y, a.z - b.z }; }
inline vec3 v3mul(vec3 a, vec3 b) { return (vec3) { a.x * b.x, a.y * b.y, a.z * b.z }; }
inline vec3 v3scale(vec3 a, f32 s) { return (vec3) { a.x * s, a.y * s, a.z * s }; }
inline f32 v3length(vec3 a) { return sqrtf(a.x * a.x + a.y * a.y + a.z * a.z); }