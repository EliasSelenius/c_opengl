#version 330 core

/*
    Screen covering quad fragment shader
*/

// Application UBO
layout (std140) uniform Application {
    vec4 time_delta_width_height;
} app;

float getTime() {
    return app.time_delta_width_height.x;
}

vec2 getViewportSize() {
    return app.time_delta_width_height.zw;
}

// END Application UBO

uniform sampler2D u_texture;

out vec4 FragColor;
in vec2 uv;

void main() {
    FragColor = texture(u_texture, uv);
}