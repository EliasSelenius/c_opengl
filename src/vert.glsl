#version 330 core

vec2 ps[] = vec2[](
    vec2(-1, -1),
    vec2(0, 1),
    vec2(1, -1)
);

void main() {
    gl_Position = vec4(ps[gl_VertexID], 0, 1); 
}

