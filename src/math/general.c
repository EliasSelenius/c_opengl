#include "general.h"
#include "../types.h"
#include "vec.h"

f32 random(i32 seed) {
    seed = (seed << 13) ^ seed;
    return (1.0f - ((seed * (seed * seed * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);
}

// TODO: the 'quality' of randomnes is not tested here
f32 random2(i32 seed_x, i32 seed_y) {
    return random(seed_x + seed_y * 57);
}



static vec2 randomVec2(f32 x, f32 y) {
    vec2 v;
    v.x = x * 127.1f + y * 311.7f;
    v.y = x * 268.5f + y * 183.3f;

    v.x = fract(sin(v.x) * 43758.5453123f) * 2.0f - 1.0f;
    v.y = fract(sin(v.y) * 43758.5453123f) * 2.0f - 1.0f;

    return v;
} 


// One-dimensional value noise.
f32 vnoise(f32 x) {
    i32 i = (i32)x;
    f32 f = fract(x);
    f32 u = f * f * (3.0f - 2.0f * f);
    return lerp(random(i), random(i + 1), u);    
}

// Two-dimensional value noise.
f32 vnoise2(f32 x, f32 y) {
    i32 ix = (i32)x, iy = (i32)y;
    f32 fx = fract(x), fy = fract(y);

    f32 a = random2(ix    , iy    ),
        b = random2(ix + 1, iy    ),
        c = random2(ix    , iy + 1),
        d = random2(ix + 1, iy + 1);

    f32 ux = fx * fx * (3.0f - fx * 2.0f);
    f32 uy = fy * fy * (3.0f - fy * 2.0f);

    return lerp(a, b, ux) +
            (c - a) * uy * (1.0f - ux) +
            (d - b) * ux * uy;
}


// Two-dimensional gradient noise.
f32 gnoise2(f32 x, f32 y) {
    f32 ix = floor(x), iy = floor(y);
    f32 fx = fract(x), fy = fract(y);

    f32 ux = fx * fx * (-fx * 2.0f + 3.0f);
    f32 uy = fy * fy * (-fy * 2.0f + 3.0f);


    vec2 r = randomVec2(ix, iy);
    f32 d1 = r.x * fx + r.y * fy;

    r = randomVec2(ix + 1.0f, iy);
    f32 d2 = r.x * (fx - 1.0f) + r.y * fy;


    r = randomVec2(ix, iy + 1.0f);
    f32 d3 = r.x * fx + r.y * (fy - 1.0f);
    
    r = randomVec2(ix + 1.0f, iy + 1.0f);
    f32 d4 = r.x * (fx - 1.0f) + r.y * (fy - 1.0f);
    
    return lerp(lerp(d1, d2, ux), lerp(d3, d4, ux), uy);
}