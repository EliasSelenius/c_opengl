#pragma once

#include <GL.h>
#include "graphics/Framebuffer.h"
#include "graphics/shader.h"
#include "math/vec.h"

typedef struct Application {
    GLFWwindow* window;
    u32 width, height;
    f32 aspectRatio;

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

    f64 time;
    f64 deltatime;
    f64 simSpeed;
    
} Application;

extern Application app;
extern vec3 g_SunDirection;

void appExit();
void appToggleFullscreen();


void gizmoColor(f32 red, f32 green, f32 blue);
void gizmoPoint(vec3 pos);
void gizmoLine(vec3 start, vec3 end);