#version 330 core

layout (std140) uniform Camera {
    mat4 view;
    mat4 projection;
} camera;

layout (points) in;
layout (line_strip, max_vertices = 2) out;

in Fragdata {
    vec3 fragpos;
    vec3 normal;
    vec4 color;
} v[];


void main() {
    
    //vec4 triCenter = (gl_in[0].gl_Position + gl_in[1].gl_Position + gl_in[2].gl_Position) / 3.0;
    
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();
    
    vec4 no = camera.projection * vec4(v[0].normal, 0);

    gl_Position = gl_in[0].gl_Position + no;
    EmitVertex();

    EndPrimitive();
}