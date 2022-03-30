
#ifndef SUN_IMPL
#define SUN_IMPL

layout (std140) uniform Sun {
    // view space direction
    vec3 vDirection;
    vec3 color;
} sun;

#endif