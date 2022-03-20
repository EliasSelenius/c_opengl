#version 460 core

#include "camera.glsl"

layout (std140) uniform Model {
    mat4 model;
};

layout (location = 0) in vec3 a_Pos;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec4 a_Color;

out Fragdata {
    vec3 fragpos;
    vec3 normal;
    vec4 color;
} v;

void main() {
    mat4 view_model = camera.view * model;
    vec4 pos = view_model * vec4(a_Pos, 1.0);

    v.fragpos = pos.xyz;
    v.normal = normalize((view_model * vec4(a_Normal, 0.0)).xyz);
    // v.color = a_Color;
    v.color = vec4(float(gl_DrawID) / 3.0);

    gl_Position = camera.projection * pos;
}
