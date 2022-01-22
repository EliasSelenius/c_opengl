
#ifndef CAM_IMPL
#define CAM_IMPL

layout (std140) uniform Camera {
    mat4 view;
    mat4 projection;
} camera;


#endif