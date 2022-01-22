#version 330 core

/*
    Screen covering quad fragment shader
*/

#include "app.glsl"

uniform sampler2D u_texture;

out vec4 FragColor;
in vec2 uv;

void main() {
    FragColor = texture(u_texture, uv);
}