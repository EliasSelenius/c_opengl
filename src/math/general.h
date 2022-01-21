#pragma once

#include "../types.h"
#include <math.h>

inline f32 lerp(f32 x, f32 y, f32 t) {
    return x + (y - x) * t;
}

inline f32 fract(f32 x) {
    return x - floor(x);
}

// Generates a random number in the range [-1..1]
f32 random(i32 seed);
f32 random2(i32 seed_x, i32 seed_y);

f32 vnoise(f32 x);
f32 vnoise2(f32 x, f32 y);

f32 gnoise2(f32 x, f32 y);