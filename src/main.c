#include <GL.h>
#include "graphics/shader.h"
#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "fileIO.h"
#include "String.h"
#include "math/vec.h"
#include "math/matrix.h"
#include "graphics/Mesh.h"

#include "graphics/UBO.h"
#include "graphics/glUtils.h"


/*
    TODOs:
        - projection
        - view
        - transform matrix
        - camera
    

*/



void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    printf("resize: %d * %d\n", width, height);
    glViewport(0, 0, width, height);
}


int main() {

    if (!glfwInit())
        return -1;

    GLFWwindow* window = glfwCreateWindow(1600, 900, "Test window", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }


    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);


    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    u32 vlen, flen;
    char* vert = fileread("src/graphics/shaders/vert.glsl", &vlen);
    char* frag = fileread("src/graphics/shaders/frag.glsl", &flen);
    //printf("%s\n\n\n", vert);
    //printf("%s\n\n\n", frag);
    u32 shader = shaderCreate(vert, vlen, frag, flen);
    free(vert);
    free(frag);
    

    glUseProgram(shader);

    mat4 cameraMats[2];
    mat4SetIdentity(&cameraMats[0]);
    mat4SetIdentity(&cameraMats[1]);

    u32 camBuffer = bufferCreate(cameraMats, sizeof(mat4) * 2);

    Ublock* camUbo = ublockGetByName("Camera");
    ublockBindBuffer(camUbo, camBuffer);

    ublockPrintAll();


    vertex data[] = {
        {-1, -1, 0,    1, 0, 0, 1},
        {0, 1, 0,      0, 1, 0, 1},
        {1, -1, 0,     0, 0, 1, 1}
    };

    u32 ind[] = { 0, 2, 1 };

    Mesh mesh;
    meshCreate(3, data, 3, ind, &mesh);    

    while (!glfwWindowShouldClose(window)) {

        glClearColor(0,1,0, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        meshRender(&mesh);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    meshDelete(&mesh);

    glfwDestroyWindow(window);

    glfwTerminate();
    return 0;
}