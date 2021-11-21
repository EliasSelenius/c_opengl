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
Camera g_Camera;
f64 mouse_x, mouse_y, pmouse_x, pmouse_y, dmouse_x, dmouse_y;
vec2 wasd;

// test gameobject
typedef struct Gameobject {
    Transform transform;
    Mesh* mesh;
} Gameobject;


void gameobjectInit(Mesh* mesh, Gameobject* out_result) {
    out_result->mesh = mesh;
    transformSetDefaults(&out_result->transform);
}

void gameobjectRender(Gameobject* gameobject) {
    mat4 model;
    transformToMatrix(&gameobject->transform, &model);
    bufferInit(app.modelUBO->bufferId, &model, sizeof(mat4));
    meshRender(gameobject->mesh);
}

Gameobject planeObject;



void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    printf("resize: %d * %d\n", width, height);
    glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    //printf("%i, %i, %i, %i\n", key, scancode, action, mods);

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
    glfwSetKeyCallback(app.window, key_callback);


    initUBO(&app.cameraUBO, "Camera", sizeof(mat4) * 2);
    initUBO(&app.modelUBO, "Model", sizeof(mat4));
    

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // TODO look up glMultiDrawElements()

    return 1;
}

void appExit() {
    glfwSetWindowShouldClose(app.window, 1);
}

static void drawframe() {
    glClearColor(0,1,1, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    //quatFromAxisAngle(&(vec3){0,0,1}, glfwGetTime() * 3.0f, &cam.transform.rotation);

    g_Camera.transform.position.x += wasd.x / 10.0f;
    g_Camera.transform.position.z += wasd.y / 10.0f;

    cameraUse(&g_Camera);

    quat q;
    quatFromAxisAngle(&(vec3){0, 1, 0}, glfwGetTime(), &q);
    //planeObject.transform.rotation = q;
    planeObject.transform.position.x = round(g_Camera.transform.position.x);
    planeObject.transform.position.z = round(g_Camera.transform.position.z);

    gameobjectRender(&planeObject);
}

static void updateInput() {

    pmouse_x = mouse_x;
    pmouse_y = mouse_y;
    glfwGetCursorPos(app.window, &mouse_x, &mouse_y);
    dmouse_x = mouse_x - pmouse_x;
    dmouse_y = mouse_y - pmouse_y;
    //printf("Mouse: %f, %f  delta: %f, %f\n", mouse_x, mouse_y, dmouse_x, dmouse_y);


    wasd = (vec2) { 0, 0 };
    if (glfwGetKey(app.window, GLFW_KEY_W)) wasd.y += 1;
    if (glfwGetKey(app.window, GLFW_KEY_S)) wasd.y -= 1;
    if (glfwGetKey(app.window, GLFW_KEY_A)) wasd.x += 1;
    if (glfwGetKey(app.window, GLFW_KEY_D)) wasd.x -= 1;
    if (wasd.x != 0.0f || wasd.y != 0.0f) {
        vec2Normalize(&wasd);
    }
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
    cameraInit(&g_Camera, 3.14 / 2.0, 0.1, 100.0);
    cameraUse(&g_Camera);
    

    // Mesh
    vertex data[] = {
        {-1, -1, 0,   0,0,0,   1, 0, 0, 1},
        {0, 1, 0,     0,0,0,   0, 1, 0, 1},
        {1, -1, 0,    0,0,0,   0, 0, 1, 1}
    };
    u32 ind[] = { 0, 2, 1 };
    Mesh mesh;
    meshCreate(3, data, 3, ind, &mesh);


    // plane
    Mesh plane;
    MeshData planeData;
    genPlane(&planeData, 40);
    meshCreate(planeData.vertexCount, planeData.vertices, planeData.indexCount, planeData.indices, &plane);

    
    mat4 model, planeModel;
    mat4SetIdentity(&model);


    gameobjectInit(&plane, &planeObject);
    planeObject.transform.position = (vec3) {0, -3, -4};


    while (!glfwWindowShouldClose(app.window)) {
        updateInput();
        drawframe();

        glfwSwapBuffers(app.window);
        glfwPollEvents();
    }

    glfwDestroyWindow(app.window);
    glfwTerminate();
}

