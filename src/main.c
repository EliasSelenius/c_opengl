#include <GL.h>
#include "shader.h"
#include <stdio.h>
#include "types.h"

#include "fileIO.h"

#include "String.h"

#include "vec.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    printf("resize: %d * %d\n", width, height);
    glViewport(0, 0, width, height);
}

void makeBuffer() {
    u32 b;
    glGenBuffers(1, &b);
    glBindBuffer(GL_ARRAY_BUFFER, b);

    vec2 data[] = {
        {-1, -1},
        {0, 1},
        {1, -1}
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
}

int main() {

    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    window = glfwCreateWindow(1600, 900, "Test window", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }


    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);


    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


    char* vert = fileread("src/vert.glsl");
    char* frag = fileread("src/frag.glsl");
    //printf("%s\n\n\n", vert);
    //printf("%s\n\n\n", frag);
    u32 shader = shaderCreate(vert, frag);
    free(vert);
    free(frag);
    

    glUseProgram(shader);

    while (!glfwWindowShouldClose(window)) {

        glClearColor(0,1,0, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    return 0;
}