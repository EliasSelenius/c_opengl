#version 460 core

#include "camera.glsl"


layout (location = 0) out vec3 FragPos;
layout (location = 1) out vec3 FragNormal;
layout (location = 2) out vec3 FragColor;

uniform vec3 u_Color;

in Fragdata {
    vec3 fragpos;
    vec3 normal;
    vec4 color;
} frag;

void main() {
    FragPos = frag.fragpos;
    FragNormal = normalize(frag.normal);
    FragColor = frag.color.rgb;
    // FragColor = u_Color;
    // FragColor = vec3(float(gl_DrawID) / 3.0);
}
