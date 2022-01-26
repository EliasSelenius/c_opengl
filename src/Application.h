#pragma once

#include <GL.h>
#include "graphics/UBO.h"
#include "graphics/Framebuffer.h"

typedef struct Application {
    GLFWwindow* window;
    u32 width, height;

    u32 defShader, waterShader, scqShader;

    u32 gPassShader;
    u32 dirlightShader;
    u32 pointlightShader;

    Framebuffer* fbo;
    Framebuffer* gBuffer;
    Framebuffer* hdrBuffer;

    Ublock* appUBO;
    Ublock* cameraUBO;
    Ublock* modelUBO;

    f64 time, prevtime, deltatime;
    
} Application;

extern Application app;

void appExit();
void appToggleFullscreen();
