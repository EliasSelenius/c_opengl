#pragma once

#include <GL.h>
#include "graphics/Framebuffer.h"
#include "graphics/shader.h"

typedef struct Application {
    GLFWwindow* window;
    u32 width, height;

    u32 waterShader;
    u32 scqShader;
    u32 skyboxShader;

    u32 gPassShader;
    u32 dirlightShader;
    u32 pointlightShader;
    u32 hdr2ldrShader;

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
