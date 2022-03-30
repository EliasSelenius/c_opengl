#version 430 core

#include "gBuffer.glsl"
#include "camera.glsl"
#include "PBR.glsl"
#include "sun.glsl"

in vec2 uv;

out vec4 LightColor;

void main() {
    gBufferData data = gBufferRead(uv);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, data.albedo, data.metallic);

    vec3 V = normalize(-data.pos);


    LightColor = vec4(CalcDirlight(
        /* light direction */ sun.vDirection,
        /* light color     */ sun.color,
        /*                 */ F0,
        /* surface normal  */ data.normal,
        /*                 */ V,
        /* surface color   */ data.albedo,
        /*                 */ data.roughness,
        /*                 */ data.metallic), 1.0);

    // ambient light
    LightColor += vec4(data.albedo, 0.0) * 0.1;
}