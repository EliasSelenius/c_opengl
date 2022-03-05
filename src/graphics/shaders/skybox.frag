#version 330 core

out vec4 FragColor;

in vec3 pos;

void main() {
    vec3 normal = normalize(pos);
    FragColor = vec4(normal, 1);
}