#pragma once

#include <GL.h>
#include "graphics/Framebuffer.h"
#include "graphics/shader.h"
#include "math/vec.h"

typedef struct Application {
    GLFWwindow* window;
    u32 width, height;

    u32 gizmoShader;
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
    Ublock* sunUBO;

    f64 time, prevtime, deltatime;
    
} Application;

extern Application app;
extern vec3 g_SunDirection;

void appExit();
void appToggleFullscreen();
