#pragma once

#include "../math/matrix.h"
#include "../prelude.h"
#include "../math/Transform.h"
#include "../Application.h"

typedef struct Camera {
    Transform transform;

    f32 fieldOfView;
    f32 nearPlane, farPlane;
    
    f32 nearPlaneWidth; // it's actually half the near planes width
    f32 nearPlaneHeight; // it's actually half the near planes height

    mat4 view;
    mat4 projection;
} Camera;

void cameraInit(Camera* cam, f32 fov, f32 near, f32 far);

void cameraUpdateMatrices(Camera* cam);
void cameraUse(Camera* cam);

vec3 cameraRay(Camera* cam, vec2 ndc);

inline vec2 screen2ndc(f32 coord_x, f32 coord_y) {
    f32 x = ((coord_x / (app.width-1)) - 0.5) * 2.0;
    f32 y = ((coord_y / (app.height-1)) - 0.5) * 2.0;

    return (vec2) { x, -y };
}