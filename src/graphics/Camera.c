#include "Camera.h"
#include "../math/matrix.h"
#include "../Application.h"
#include "glUtils.h"
#include "../math/general.h"

void cameraInit(Camera* cam, f32 fov, f32 near, f32 far) {
    transformSetDefaults(&cam->transform);

    cam->fieldOfView = fov;
    cam->nearPlane = near;
    cam->farPlane = far;

    cam->nearPlaneHeight = near / (tan(HALF_PI - fov / 2.0));
    cam->nearPlaneWidth = cam->nearPlaneHeight * app.aspectRatio;

    mat4SetIdentity(&cam->view);
    mat4SetIdentity(&cam->projection);
}

/*
    updates the projection and view matrices using the camera properties
*/
void cameraUpdateMatrices(Camera* cam) {
    // projection
    mat4Perspective(cam->fieldOfView, app.aspectRatio, cam->nearPlane, cam->farPlane, &cam->projection);

    // view
    transformToMatrix(&cam->transform, &cam->view);
    cam->view.row1.x *= -1;
    cam->view.row1.y *= -1;
    cam->view.row1.z *= -1;

    cam->view.row3.x *= -1;
    cam->view.row3.y *= -1;
    cam->view.row3.z *= -1;

    vec3 xaxis = cam->view.row1.xyz;
    vec3 yaxis = cam->view.row2.xyz;
    vec3 zaxis = cam->view.row3.xyz;

    cam->view = (mat4) {
        // the axises here are on the columns (insted of rows) because the transpose of an orthonormalized matrix is the same as its inverse
        xaxis.x, yaxis.x, zaxis.x, 0,
        xaxis.y, yaxis.y, zaxis.y, 0,
        xaxis.z, yaxis.z, zaxis.z, 0,

        -vec3Dot(&xaxis, &cam->transform.position),
        -vec3Dot(&yaxis, &cam->transform.position),
        -vec3Dot(&zaxis, &cam->transform.position), 1
    };
}

/*
    updates and uploads matrices to UBO
*/
void cameraUse(Camera* cam) {

    // camera
    cameraUpdateMatrices(cam);
    bufferInit(app.cameraUBO->bufferId, &cam->view, sizeof(mat4) * 2);

    
    // sun direction in view space
    vec3 ld = g_SunDirection;
    ld = (vec3) {
        ld.x * cam->view.row1.x   +   ld.y * cam->view.row2.x   +   ld.z * cam->view.row3.x,
        ld.x * cam->view.row1.y   +   ld.y * cam->view.row2.y   +   ld.z * cam->view.row3.y,
        ld.x * cam->view.row1.z   +   ld.y * cam->view.row2.z   +   ld.z * cam->view.row3.z
    };

    bufferSubData(app.sunUBO->bufferId, 0, &ld, sizeof(ld));
    
}

vec3 cameraRay(Camera* cam, vec2 ndc) {
    vec3 ray = {0};

    // NOTE: the first, second and third columns of the cameras view matrix is the side, up and forward vectors respectively.

    ray.x = -cam->view.row1.z * cam->nearPlane;
    ray.y = -cam->view.row2.z * cam->nearPlane;
    ray.z = -cam->view.row3.z * cam->nearPlane;

    ray.x += cam->view.row1.y * cam->nearPlaneHeight * ndc.y;
    ray.y += cam->view.row2.y * cam->nearPlaneHeight * ndc.y;
    ray.z += cam->view.row3.y * cam->nearPlaneHeight * ndc.y;

    ray.x += cam->view.row1.x * cam->nearPlaneWidth * ndc.x;
    ray.y += cam->view.row2.x * cam->nearPlaneWidth * ndc.x;
    ray.z += cam->view.row3.x * cam->nearPlaneWidth * ndc.x;

    return v3normalize(ray);
}
