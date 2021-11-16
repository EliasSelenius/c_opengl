#include <GL.h>
#include "Application.h"
#include "types.h"
#include "fileIO.h"

#include <stdio.h>
#include <stdlib.h>

#include "math/matrix.h"
#include "math/vec.h"
#include "math/quat.h"

#include "graphics/Mesh.h"
#include "graphics/UBO.h"
#include "graphics/shader.h"
#include "graphics/glUtils.h"
#include "graphics/Camera.h"

Application app;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    printf("resize: %d * %d\n", width, height);
    glViewport(0, 0, width, height);
}

static void initUBO(Ublock** ubo, char* name, u32 size) {
    *ubo = ublockGetByName(name);
    u32 buffer = bufferCreate(NULL, size);
    ublockBindBuffer(*ubo, buffer);
}

int appInit() {

    if (!glfwInit())
        return -1;

    app.window = glfwCreateWindow(1600, 900, "Test window", NULL, NULL);
    if (!app.window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(app.window);
    gladLoadGL(glfwGetProcAddress);

    glfwSetFramebufferSizeCallback(app.window, framebuffer_size_callback);


    initUBO(&app.cameraUBO, "Camera", sizeof(mat4) * 2);
    initUBO(&app.modelUBO, "Model", sizeof(mat4));
    

    return 1;
}

void appExit() {
    glfwSetWindowShouldClose(app.window, 1);
}

void drawframe() {
    glClearColor(0,1,1, 1);
    glClear(GL_COLOR_BUFFER_BIT);
}

int main() {
    if (!appInit()) return -1;

    // Shader:
    u32 vlen, flen;
    char* vert = fileread("src/graphics/shaders/vert.glsl", &vlen);
    char* frag = fileread("src/graphics/shaders/frag.glsl", &flen);
    //printf("%s\n\n\n", vert);
    //printf("%s\n\n\n", frag);
    u32 shader = shaderCreate(vert, vlen, frag, flen);
    free(vert);
    free(frag);
    
    glUseProgram(shader);


    // Camera

    

    Camera cam;
    cameraInit(&cam, 3.14 / 2.0, 0.1, 100.0);
    cameraUse(&cam);
    

    // Mesh
    vertex data[] = {
        {-1, -1, 0,    1, 0, 0, 1},
        {0, 1, 0,      0, 1, 0, 1},
        {1, -1, 0,     0, 0, 1, 1}
    };
    u32 ind[] = { 0, 2, 1 };
    Mesh mesh;
    meshCreate(3, data, 3, ind, &mesh);

    
    mat4 model;
    mat4SetIdentity(&model);


    while (!glfwWindowShouldClose(app.window)) {

        drawframe();

        cameraUse(&cam);

        pushMatrix();
 
        quat q;
        quatFromAxisAngle(&(vec3){0, 1, 0}, glfwGetTime() * 3.0f, &q);
        rotate(q);
        translate((vec3){0, 0, -10});
 
        popMatrix(&model);

        bufferInit(app.modelUBO->bufferId, &model, sizeof(mat4));
        meshRender(&mesh);



        glfwSwapBuffers(app.window);
        glfwPollEvents();
    }

    glfwDestroyWindow(app.window);
    glfwTerminate();
}

