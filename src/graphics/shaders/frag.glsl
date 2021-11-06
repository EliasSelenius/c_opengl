#version 330 core

out vec4 FragColor;

in Fragdata {
    vec4 color;
} frag;

void main() {
    FragColor = frag.color;
}