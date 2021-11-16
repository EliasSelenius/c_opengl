#pragma once

#include <GL.h>
#include "graphics/UBO.h"

typedef struct Application {
    GLFWwindow* window;
    Ublock* cameraUBO;
    Ublock* modelUBO;
} Application;

extern Application app;
