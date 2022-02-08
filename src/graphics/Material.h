#pragma once

#include "../math/vec.h"
#include "../types.h"

typedef struct Material {
    vec3 albedo;
    u32 roughness;
    u32 metallic;
} Material;

