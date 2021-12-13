#version 330 core

/*
    Screen covering quad vertex shader
*/

vec2 positions[] = vec2[](
    vec2(-1, -1),
    vec2(1, -1),
    vec2(-1, 1),

    vec2(1, -1),
    vec2(1, 1),
    vec2(-1, 1)
);

out vec2 uv;

void main() {
    vec2 pos = positions[gl_VertexID];
    uv = (pos + vec2(1.0)) * 0.5;
    gl_Position = vec4(pos, 0.0, 1.0);
}