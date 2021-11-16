#include "Camera.h"
#include "../math/matrix.h"
#include "../Application.h"
#include "glUtils.h"


void cameraInit(Camera* cam, f32 fov, f32 near, f32 far) {
    cam->fieldOfView = fov;
    cam->nearPlane = near;
    cam->farPlane = far;

    mat4SetIdentity(&cam->view);
    mat4SetIdentity(&cam->projection);
}

/*
    updates the projection and view matrices using the camera properties
*/
void cameraUpdateMatrices(Camera* cam) {
    int w, h;
    glfwGetFramebufferSize(app.window, &w, &h);

    mat4Perspective(cam->fieldOfView, (f32)w / h, cam->nearPlane, cam->farPlane, &cam->projection);
}

/*
    updates and uploads matrices to UBO
*/
void cameraUse(Camera* cam) {
    cameraUpdateMatrices(cam);

    bufferInit(app.cameraUBO->bufferId, &cam->view, sizeof(mat4) * 2);
}