#version 330 core

in FragData {
    vec3 color;
} input;

out vec3 FragColor;

void main() {
    FragColor = input.color;
}