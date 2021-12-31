#include "vec.h"
#include <math.h>

void vec2Normalize(vec2* v) {
    f32 l = sqrt(v->x * v->x + v->y * v->y);
    v->x /= l;
    v->y /= l;
}


void vec3Add(vec3* a, vec3 b) {
    a->x += b.x;
    a->y += b.y;
    a->z += b.z;
}

void vec3Sub(vec3* a, vec3 b) {
    a->x -= b.x;
    a->y -= b.y;
    a->z -= b.z;
}

void vec3Scale(vec3* a, f32 scalar) {
    a->x *= scalar;
    a->y *= scalar;
    a->z *= scalar;
}

f32 vec3Dot(vec3* a, vec3* b) {
    return a->x * b->x + a->y * b->y + a->z * b->z;
}

void vec3Normalize(vec3* v) {
    f32 l = sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
    v->x /= l;
    v->y /= l;
    v->z /= l;
}

void vec3Cross(vec3* a, vec3* b, vec3* out_result) {
    *out_result = (vec3){
        a->y * b->z - a->z * b->y,
        a->z * b->x - a->x * b->z,
        a->x * b->y - a->y * b->x
    };
}

f32 vec3Length(vec3 v) { return sqrt(v.x*v.x + v.y*v.y + v.z*v.z); }

f32 vec4Dot(vec4* a, vec4* b) {
    return a->x * b->x + a->y * b->y + a->z * b->z + a->w * b->w; 
}