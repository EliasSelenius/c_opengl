#include "vec.h"

f32 vec4dot(vec4* a, vec4* b) {
    return a->x * b->x + a->y * b->y + a->z * b->z + a->w * b->w; 
}