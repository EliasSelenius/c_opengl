#pragma once

#include <GL.h>
#include "graphics/UBO.h"

typedef struct Application {
    GLFWwindow* window;

    u32 defShader, waterShader;

    Ublock* appUBO;
    Ublock* cameraUBO;
    Ublock* modelUBO;
} Application;

extern Application app;

void appExit();
