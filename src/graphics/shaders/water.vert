#version 330 core

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

layout (std140) uniform Model {
    mat4 model;
};

layout (std140) uniform Camera {
    mat4 view;
    mat4 projection;
} camera;

layout (location = 0) in vec3 a_Pos;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec4 a_Color;

out Fragdata {
    vec3 fragpos;
    vec3 normal;
    vec4 color;
} v;

float waveHeight(vec2 pos) {
    return sin((pos.x - pos.y) / 3.0 + (getTime())) * 0.5;
    // return 0;
}

void main() {
    vec4 wpos = model * vec4(a_Pos, 1);
    wpos.y += waveHeight(wpos.xz);

    // world pos to view pos
    vec4 vpos = camera.view * wpos;

    v.fragpos = vpos.xyz;
    v.color = a_Color;

    // calculate normal
    //v.normal = (camera.view * vec4(0, 1, 0, 0)).xyz;


    {
        /*
               o v1
             / | \
            /  |  \
        v4 o---o---o v2
            \  |  /
             \ | /
               o v3
        */

        vec3 v1 = vec3(0, 0, 1);
        vec3 v2 = vec3(1, 0, 0);
        vec3 v3 = vec3(0, 0, -1);
        vec3 v4 = vec3(-1, 0, 0);

        v1.y = waveHeight(wpos.xz + v1.xz);
        v2.y = waveHeight(wpos.xz + v2.xz);
        v3.y = waveHeight(wpos.xz + v3.xz);
        v4.y = waveHeight(wpos.xz + v4.xz);

        vec3 n1 = cross(v1, v2);
        vec3 n2 = cross(v2, v3);
        vec3 n3 = cross(v3, v4);
        vec3 n4 = cross(v4, v1);

        v.normal = (camera.view * vec4(normalize(n1 + n2 + n3 + n4), 0.0)).xyz;
    }



    gl_Position = camera.projection * camera.view * wpos;
}
