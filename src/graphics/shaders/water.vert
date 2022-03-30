#version 330 core

#include "app.glsl"
#include "camera.glsl"

#define PI 3.14159265359

uniform vec2 u_worldPos;


layout (std140) uniform Waves {
    vec4 wave_dir_steepness_length[3];
};


layout (location = 0) in vec3 a_Pos;
layout (location = 1) in vec3 a_Normal;

out Fragdata {
    vec3 fragpos;
    vec3 normal;
} v;


vec3 gerstnerWave(vec2 coord, vec2 waveDir, float waveSteepness, float waveLength, inout vec3 tangent, inout vec3 binormal) {
    float k = 2.0 * PI / waveLength;
    float c = sqrt(9.8 / k);
    vec2 d = normalize(waveDir);
    float f = k * (dot(d, coord) - c * Time);
    float a = waveSteepness / k;


    tangent += vec3(
        -d.x * d.x * waveSteepness * sin(f),
        d.x * waveSteepness * cos(f),
        -d.x * d.y * waveSteepness * sin(f)
    );
    binormal += vec3(
        -d.x * d.y * waveSteepness * sin(f),
        d.y * waveSteepness * cos(f),
        -d.y * d.y * waveSteepness * sin(f)
    );

    return vec3(
        d.x * cos(f) * a,
              sin(f) * a,
        d.y * cos(f) * a
    );
}


void main() {
    // world pos
    vec4 wpos = vec4(a_Pos, 1);
    wpos.xz += u_worldPos;

    vec3 tangent = vec3(1, 0, 0);
    vec3 binormal = vec3(0, 0, 1);

    vec2 coord = wpos.xz;
    // TODO: visualize these wave properties with gizmos
    // wpos.xyz += gerstnerWave(coord, vec2(1, 1), 0.25, 60, tangent, binormal);
    // wpos.xyz += gerstnerWave(coord, vec2(1, 0.6), 0.25, 31, tangent, binormal);
    // wpos.xyz += gerstnerWave(coord, vec2(1, 1.3), 0.25, 18, tangent, binormal);

    for (int i = 0; i < 3; i++) {
        wpos.xyz += gerstnerWave(coord,
            wave_dir_steepness_length[i].xy,
            wave_dir_steepness_length[i].z,
            wave_dir_steepness_length[i].w,
            tangent, binormal);
    }
 
    // output
    v.fragpos = (camera.view * wpos).xyz;
    v.normal = (camera.view * vec4(normalize(cross(binormal, tangent)), 0)).xyz;
    gl_Position = camera.projection * camera.view * wpos;
}
