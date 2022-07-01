#version 460 core

#include "camera.glsl"

layout (std140) uniform Model {
    mat4 model;
};


struct Mat {
    vec4 albedo;
    // vec2 metallic_roughness;
    // float metallic;
    // float roughness;
};

layout (std140) uniform Material {
    Mat materials[3];
};


layout (location = 0) in vec3 a_Pos;
layout (location = 1) in vec3 a_Normal;
// TODO: per instance attributes
// layout (location = 2) in vec4 a_Material_Albedo;


out Fragdata {
    vec3 fragpos;
    vec3 normal;
    vec4 color;
} v;

void main() {
    mat4 view_model = camera.view * model;
    vec4 pos = view_model * vec4(a_Pos, 1.0);

    v.fragpos = pos.xyz;
    v.normal = normalize(mat3(view_model) * a_Normal);

    // v.color = vec4(float(gl_DrawID) / 3.0);

    v.color = materials[gl_DrawID].albedo;

    gl_Position = camera.projection * pos;
}
