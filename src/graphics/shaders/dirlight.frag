#version 430 core

#include "gBuffer.glsl"

in vec2 uv;

out vec4 LightColor;

void main() {
    gBufferData data = gBufferRead(uv);

    LightColor = vec4(data.albedo, 1.0);

    // LightColor = vec4(0.0, 1.0, 0.0, 1.0);
}