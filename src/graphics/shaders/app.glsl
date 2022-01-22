
#ifndef APP_IMPL
#define APP_IMPL

layout (std140) uniform Application {
    vec4 time_delta_width_height;
} app;

#define Time app.time_delta_width_height.x
#define Deltatime app.time_delta_width_height.y

#define ViewportSize app.time_delta_width_height.zw
#define Width app.time_delta_width_height.z
#define Height app.time_delta_width_height.w

float getTime() {
    return app.time_delta_width_height.x;
}

vec2 getViewportSize() {
    return app.time_delta_width_height.zw;
}

#endif