#version 330 core

#include "camera.glsl"

layout (location = 0) in vec3 a_Pos;

out vec3 pos;
void main() {
    pos = a_Pos;
    
    mat4 rotview = mat4(mat3(camera.view)); // remove translation from view matrix
    vec4 clipPos = camera.projection * rotview * vec4(a_Pos, 1.0);

    // Note the xyww trick here that ensures the depth value of the rendered fragments always end up at 1.0, the maximum
    // depth value, as described in the cubemap chapter of learnOpenGL.com.
    // Do note that we need to change the depth comparison function to GL_LEQUAL:
    gl_Position = clipPos.xyww;
}