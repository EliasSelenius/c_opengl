#include <GL.h>
#include "Application.h"
#include "types.h"
#include "fileIO.h"
#include "String.h"
#include "List.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "math/matrix.h"
#include "math/vec.h"
#include "math/quat.h"

#include "graphics/Mesh.h"
#include "graphics/UBO.h"
#include "graphics/shader.h"
#include "graphics/glUtils.h"
#include "graphics/Camera.h"

#include "Gameobject.h"

#include "graphics/obj.h"


/*
    - water shader
    - model loading
    - mesh groups
*/

Application app;
Camera g_Camera;
f64 mouse_x, mouse_y, pmouse_x, pmouse_y, dmouse_x, dmouse_y;
vec2 wasd;


Gameobject planeObject, triangleObject;



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


    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // TODO look up glMultiDrawElements()


    return 1;
}

void appExit() {
    glfwSetWindowShouldClose(app.window, 1);
}

static void cameraFlyControll() {
    mat4 cam_model;
    transformToMatrix(&g_Camera.transform, &cam_model);
    vec3 forward = cam_model.row3.xyz;
    vec3 left = cam_model.row1.xyz;

    f32 scale = 1.0f;
    if (glfwGetKey(app.window, GLFW_KEY_LEFT_SHIFT)) {
        scale = 4.0f;
    }

    vec3Scale(&cam_model.row3.xyz, wasd.y / 10.0f * scale);
    vec3Scale(&cam_model.row1.xyz, wasd.x / 10.0f * scale);

    vec3Add(&g_Camera.transform.position, cam_model.row3.xyz);
    vec3Add(&g_Camera.transform.position, cam_model.row1.xyz);


    if (glfwGetMouseButton(app.window, GLFW_MOUSE_BUTTON_RIGHT)) {
        transformRotateAxisAngle(&g_Camera.transform, left, -dmouse_y / 100.0);
        transformRotateAxisAngle(&g_Camera.transform, (vec3) { 0, 1, 0 }, dmouse_x / 100.0);

    }


}

static void drawframe() {
    glClearColor(0,1,1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //quatFromAxisAngle(&(vec3){0,0,1}, glfwGetTime() * 3.0f, &cam.transform.rotation);

    cameraFlyControll();
    cameraUse(&g_Camera);

    glUseProgram(app.waterShader);
    // water
    planeObject.transform.position.x = round(g_Camera.transform.position.x);
    planeObject.transform.position.z = round(g_Camera.transform.position.z);
    gameobjectRender(&planeObject);
    
    glUseProgram(app.defShader);
    // triangle
    transformRotateAxisAngle(&triangleObject.transform, (vec3){0,1,0}, 0.1f);
    gameobjectRender(&triangleObject);


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

    //shaderReadFromFile("src/graphics/shaders/def.frag");


    /*{ // dynamic array test
        vec3* list = listCreate(vec3);
        listAdd(list, (&(vec3) { 1, 0, 0}));
        listAdd(list, (&(vec3) { 0, 1, 0}));
        listAdd(list, (&(vec3) { 0, 0, 1}));
        listAdd(list, (&(vec3) { 1, 1, 1}));

        for (int i = 0; i < listLength(list); i++) {
            printf("[%i] = %f, %f, %f\n", i, list[i].x, list[i].y, list[i].z);
        }
    }*/

    if (!appInit()) return -1;

    MeshData objData;
    objLoad("src/models/pyramid.obj", &objData);

    // load shaders:
    app.defShader = shaderLoad("def");
    app.waterShader = shaderLoad("water");

    glUseProgram(app.defShader);


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
    genPlane(&planeData, 50);
    meshCreate(planeData.vertexCount, planeData.vertices, planeData.indexCount, planeData.indices, &plane);

    
    mat4 model, planeModel;
    mat4SetIdentity(&model);


    gameobjectInit(&plane, &planeObject);
    planeObject.transform.position = (vec3) {0, -3, -4};

    gameobjectInit(&mesh, &triangleObject);


    while (!glfwWindowShouldClose(app.window)) {
        updateInput();
        drawframe();

        glfwSwapBuffers(app.window);
        glfwPollEvents();
    }

    glfwDestroyWindow(app.window);
    glfwTerminate();
}

