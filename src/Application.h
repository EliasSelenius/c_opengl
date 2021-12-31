#pragma once

#include <GL.h>
#include "graphics/UBO.h"
#include "graphics/Framebuffer.h"

typedef struct Application {
    GLFWwindow* window;

    u32 defShader, waterShader, scqShader;

    Framebuffer* fbo;    

    Ublock* appUBO;
    Ublock* cameraUBO;
    Ublock* modelUBO;

    f64 time, prevtime, deltatime;
    
} Application;

extern Application app;

void appExit();
