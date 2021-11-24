#version 330 core

layout (std140) uniform Model {
    mat4 model;
};

layout (std140) uniform Camera {
    mat4 view;
    mat4 projection;
} camera;

layout (location = 0) in vec3 a_Pos;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec4 a_Color;

out Fragdata {
    vec4 color;
} v;

void main() {
    vec4 wpos = model * vec4(a_Pos, 1);

    wpos.y += sin(wpos.x - wpos.z) * 0.5;

    gl_Position = camera.projection * camera.view * wpos;
    

    v.color = a_Color;
}
