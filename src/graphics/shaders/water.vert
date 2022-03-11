#version 330 core

#include "app.glsl"
#include "camera.glsl"

#define PI 3.14159265359

uniform vec2 u_worldPos;

uniform float u_waveLength = 10.0;
uniform float u_waveSteepness = 0.5;
uniform vec2  u_waveDirection = vec2(-1, 1);

layout (location = 0) in vec3 a_Pos;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec4 a_Color;

out Fragdata {
    vec3 fragpos;
    vec3 normal;
    vec4 color;
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
    wpos.xyz += gerstnerWave(coord, vec2(1, 1), 0.25, 60, tangent, binormal);
    wpos.xyz += gerstnerWave(coord, vec2(1, 0.6), 0.25, 31, tangent, binormal);
    wpos.xyz += gerstnerWave(coord, vec2(1, 1.3), 0.25, 18, tangent, binormal);

 
    // output
    v.fragpos = (camera.view * wpos).xyz;
    v.normal = (camera.view * vec4(normalize(cross(binormal, tangent)), 0)).xyz;
    v.color = a_Color;
    gl_Position = camera.projection * camera.view * wpos;


    



    /* 
    {
        //         o v1
        //       / | \
        //      /  |  \
        //  v4 o---o---o v2
        //      \  |  /
        //       \ | /
        //         o v3
        
        vec3 v1 = vec3(0, 0, 1);
        vec3 v2 = vec3(1, 0, 0);
        vec3 v3 = vec3(0, 0, -1);
        vec3 v4 = vec3(-1, 0, 0);

        v1 += wavePoint(v1.xz + coord);
        v2 += wavePoint(v2.xz + coord);
        v3 += wavePoint(v3.xz + coord);
        v4 += wavePoint(v4.xz + coord);

        // v1.y = waveHeight(wpos.xz + v1.xz);
        // v2.y = waveHeight(wpos.xz + v2.xz);
        // v3.y = waveHeight(wpos.xz + v3.xz);
        // v4.y = waveHeight(wpos.xz + v4.xz);

        vec3 n1 = cross(v1, v2);
        vec3 n2 = cross(v2, v3);
        vec3 n3 = cross(v3, v4);
        vec3 n4 = cross(v4, v1);

        v.normal = (camera.view * vec4(normalize(n1 + n2 + n3 + n4), 0.0)).xyz;
    }
    */



}
