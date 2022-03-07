#version 330 core

#include "camera.glsl"
#include "app.glsl"

layout (location = 0) in vec3 a_Pos;

const float pointSize = 0.3;

/*
    from: https://github.com/devoln/synthgen-particles-win/blob/master/src/shader.h
*/
float calcPointSize(vec4 viewPos, float size) {
    vec4 projVox = camera.projection * vec4(vec2(size), viewPos.zw);
    return 0.25 / projVox.w * dot(ViewportSize, projVox.xy);
}

void main() {

    vec4 viewPos = camera.view * vec4(a_Pos, 1.0);

    gl_PointSize = calcPointSize(viewPos, pointSize);
    gl_Position = camera.projection * viewPos;
}