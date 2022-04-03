#pragma once

#include "../math/vec.h"
#include "../prelude.h"

typedef struct Material {
    vec3 albedo;
    u32 roughness;
    u32 metallic;
} Material;

