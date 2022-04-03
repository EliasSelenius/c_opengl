#pragma once

#include "../prelude.h"
#include <math.h>

#define PI 3.14159265359


inline f32 lerp(f32 x, f32 y, f32 t) {
    return x + (y - x) * t;
}

inline f32 fract(f32 x) {
    return x - floor(x);
}

inline f32 clamp(f32 min, f32 max, f32 t) {
    return t > max ? max : t < min ? min : t;
}

// Generates a random number in the range [-1..1]
f32 random(i32 seed);
f32 random2(i32 seed_x, i32 seed_y);

f32 vnoise(f32 x);
f32 vnoise2(f32 x, f32 y);

f32 gnoise2(f32 x, f32 y);