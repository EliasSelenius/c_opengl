#version 330 core

#include "camera.glsl"


layout (location = 0) out vec3 FragPos;
layout (location = 1) out vec3 FragNormal;
layout (location = 2) out vec3 FragColor;


in Fragdata {
    vec3 fragpos;
    vec3 normal;
    vec4 color;
} frag;

void main() {
    FragPos = frag.fragpos;
    FragNormal = normalize(frag.normal);
    FragColor = frag.color.rgb;
}
