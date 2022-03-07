#version 330 core

#include "app.glsl"
#include "camera.glsl"

uniform vec2 u_worldPos;

layout (location = 0) in vec3 a_Pos;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec4 a_Color;

out Fragdata {
    vec3 fragpos;
    vec3 normal;
    vec4 color;
} v;

float waveHeight(vec2 pos) {
    return sin((pos.x - pos.y) / 3.0 + (Time)) * 0.5;
    // return 0;
}

vec3 wavePoint(vec2 coord) {
    float c = coord.y + Time;
    vec3 res;
    res.x = coord.x;
    res.y = sin(c) * 0.3;
    res.z = coord.y + cos(c);
    return res;
}

void main() {
    // world pos
    vec4 wpos = vec4(a_Pos, 1);
    wpos.xz += u_worldPos;
    //wpos.y += waveHeight(wpos.xz);

    vec2 coord = wpos.xz;
    wpos.xyz = wavePoint(coord);

    // world pos to view pos
    v.fragpos = (camera.view * wpos).xyz;

    v.color = a_Color;

    // calculate normal
    // v.normal = (camera.view * vec4(0, 1, 0, 0)).xyz;


    {
        /*
               o v1
             / | \
            /  |  \
        v4 o---o---o v2
            \  |  /
             \ | /
               o v3
        */
        vec3 v1 = vec3(0, 0, 1);
        vec3 v2 = vec3(1, 0, 0);
        vec3 v3 = vec3(0, 0, -1);
        vec3 v4 = vec3(-1, 0, 0);

        v1 = wavePoint(v1.xz + coord);
        v2 = wavePoint(v2.xz + coord);
        v3 = wavePoint(v3.xz + coord);
        v4 = wavePoint(v4.xz + coord);

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




    gl_Position = camera.projection * camera.view * wpos;
}
