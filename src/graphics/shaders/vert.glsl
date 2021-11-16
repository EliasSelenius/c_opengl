#version 330 core

layout (std140) uniform Model {
    mat4 model;
};

layout (std140) uniform Camera {
    mat4 view;
    mat4 projection;
} camera;

layout (location = 0) in vec3 a_Pos;
layout (location = 1) in vec4 a_Color;

out Fragdata {
    vec4 color;
} v;

void main() {
    gl_Position = camera.projection * camera.view * model * vec4(a_Pos, 1); 
    v.color = a_Color;
}
