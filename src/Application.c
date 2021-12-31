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
#include "graphics/obj.h"

#include "Gameobject.h"
#include "Scene.h"
#include "Physics.h"


/*
    - water shader
    - model loading
    - mesh groups
*/

Application app;
Camera g_Camera;
f64 mouse_x, mouse_y, pmouse_x, pmouse_y, dmouse_x, dmouse_y;
vec2 wasd;

Rigidbody boatRb;
Gameobject planeObject, triangleObject, pyramidObject, boatObject, smoothBoateObject;
Scene scene;

f32 waterHeight(f32 x, f32 y) {
    f32 time = glfwGetTime();
    return sin((x - y) / 3.0 + time) * 0.5;
}

static void bufferViewportSizeToUBO(f32 res[2]) {
    glBindBuffer(GL_ARRAY_BUFFER, app.appUBO->bufferId);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(f32) * 2, sizeof(f32) * 2, res);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    printf("resize: %d * %d\n", width, height);
    glViewport(0, 0, width, height);
    framebufferResize(app.fbo, width, height);
    
    bufferViewportSizeToUBO((f32[2]){width, height});
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // printf("%i, %i, %i, %i\n", key, scancode, action, mods);
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
    
    
    
    // load shaders:
    app.defShader = shaderLoad("def");
    app.waterShader = shaderLoad("water");
    glUniform1ui(glGetUniformLocation(app.waterShader, "u_depthTexture"), 0);
    app.scqShader = shaderLoad("scq");
    glUseProgram(app.defShader);
    
    FramebufferFormat attachments[] = {
        FBF_rgba8,
        FBF_float16
    };
    
    i32 w, h;
    glfwGetFramebufferSize(app.window, &w, &h);
    app.fbo = framebufferCreate(w, h, 2, attachments, FBD_DepthComponent);
    
    
    initUBO(&app.cameraUBO, "Camera", sizeof(mat4) * 2);
    initUBO(&app.modelUBO, "Model", sizeof(mat4));
    initUBO(&app.appUBO, "Application", sizeof(f32) * 4);
    bufferViewportSizeToUBO((f32[2]){w,h});
    
    
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    // TODO look up glMultiDrawElements()
    
    
    sceneInit(&scene);
    
    
    return 1;
}

void appExit() {
    glfwSetWindowShouldClose(app.window, 1);
}

static void cameraFlyControll() {
    mat4 cam_model;
    transformToMatrix(&g_Camera.transform, &cam_model);
    
    
    if (glfwGetMouseButton(app.window, GLFW_MOUSE_BUTTON_RIGHT)) {
        // f32 l = vec3Length(cam_model.row1.xyz);
        // printf("len: %f\n", l);
        
        transformRotateAxisAngle(&g_Camera.transform, cam_model.row1.xyz, -dmouse_y / 100.0);
        transformRotateAxisAngle(&g_Camera.transform, (vec3) { 0, 1, 0 }, dmouse_x / 100.0);
    }
    
    
    
    f32 scale = 1.0f;
    if (glfwGetKey(app.window, GLFW_KEY_LEFT_SHIFT)) {
        scale = 4.0f;
    }
    
    vec3Scale(&cam_model.row3.xyz, wasd.y / 10.0f * scale);
    vec3Scale(&cam_model.row1.xyz, wasd.x / 10.0f * scale);
    
    vec3Add(&g_Camera.transform.position, cam_model.row3.xyz);
    vec3Add(&g_Camera.transform.position, cam_model.row1.xyz);
    
    
}

static void drawframe() {
    glClearColor(0,1,1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    
    cameraFlyControll();
    cameraUse(&g_Camera);
    
    glBindFramebuffer(GL_FRAMEBUFFER, app.fbo->id);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    const f32 depth_clear = 99999.0;
    glClearBufferfv(GL_COLOR, 1, &depth_clear);
    
    { // objektoj
        glUseProgram(app.defShader);
        glDisable(GL_BLEND);
        
        // triangle
        transformRotateAxisAngle(&triangleObject.transform, (vec3){0,1,0}, 0.1f);
        gameobjectRender(&triangleObject);
        
        transformRotateAxisAngle(&pyramidObject.transform, (vec3){0,1,0}, 0.02f);
        gameobjectRender(&pyramidObject);
        
        gameobjectRender(&boatObject);
        gameobjectRender(&smoothBoateObject);
        
        
        sceneRender(&scene);
    }
    
    i32 w, h;
    glfwGetFramebufferSize(app.window, &w, &h);
    
    // copy depth buffer
    glBindFramebuffer(GL_READ_FRAMEBUFFER, app.fbo->id);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0,0,w,h, 0,0,w,h, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    { // draw screen quad
        glBindTexture(GL_TEXTURE_2D, app.fbo->attachments[0].texture);
        glUseProgram(app.scqShader);
        glDisable(GL_DEPTH_TEST);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    
    
    { // water
        glUseProgram(app.waterShader);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, app.fbo->attachments[1].texture);
        glEnable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        planeObject.transform.position.x = round(g_Camera.transform.position.x);
        planeObject.transform.position.z = round(g_Camera.transform.position.z);
        gameobjectRender(&planeObject);
    }
    
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
    
    
    { // pyramid
        OBJ obj;
        objLoad("src/models/pyramid.obj", &obj);
        
        MeshData objData;
        objToFlatShadedMesh(&obj, &objData);
        
        objFree(&obj);
        
        Mesh m;
        meshCreate(objData.vertexCount, objData.vertices, objData.indexCount, objData.indices, &m);
        
        gameobjectInit(&m, &pyramidObject);
        
        pyramidObject.transform.position.y = 10;
    }
    
    { // boat
        OBJ objFile;
        objLoad("src/models/Boate.obj", &objFile);
        
        MeshData objData[2];
        objToFlatShadedMesh(&objFile, &objData[0]);
        objToSmoothShadedMesh(&objFile, &objData[1]);
        
        objFree(&objFile);
        
        Mesh m;
        meshCreate(objData[0].vertexCount, objData[0].vertices, objData[0].indexCount, objData[0].indices, &m);
        Mesh sm;
        meshCreate(objData[1].vertexCount, objData[1].vertices, objData[1].indexCount, objData[1].indices, &sm);
        
        
        
        gameobjectInit(&m, &boatObject);
        gameobjectInit(&sm, &smoothBoateObject);
        
        boatObject.transform.position = (vec3) { 20, -5, 0 };
        smoothBoateObject.transform.position = (vec3) { 30, -6, 0 };
    }
    
    { // boat Rb
        boatRb = (Rigidbody) {
            .mass = 1.0f,
            .rotational_velocity = (quat) { 0.0f, 0.0f, 0.0f, 1.0f }
        };
        
        //quatFromAxisAngle(&(vec3) {0, 1, 0}, 0.01f, &boatRb.rotational_velocity);
    }
    
    { // triangle
        vertex data[] = {
            {-1, -1, 0,   0,0,0,   1, 0, 0, 1},
            {0, 1, 0,     0,0,0,   0, 1, 0, 1},
            {1, -1, 0,    0,0,0,   0, 0, 1, 1}
        };
        u32 ind[] = { 0, 2, 1 };
        Mesh mesh;
        meshCreate(3, data, 3, ind, &mesh);
        gameobjectInit(&mesh, &triangleObject);
    }
    
    { // water
        // plane
        Mesh plane;
        MeshData planeData;
        genPlane(&planeData, 1000);
        meshCreate(planeData.vertexCount, planeData.vertices, planeData.indexCount, planeData.indices, &plane);
        
        gameobjectInit(&plane, &planeObject);
        planeObject.transform.position = (vec3) {0, -3, -4};
    }
    
    
    // Camera
    cameraInit(&g_Camera, 3.14 / 2.0, 0.1, 1000.0);
    cameraUse(&g_Camera);
    
    
    while (!glfwWindowShouldClose(app.window)) {
        
        app.prevtime = app.time;
        app.time = glfwGetTime();
        app.deltatime = app.time - app.prevtime;
        
        f32 time = app.time;
        glBindBuffer(GL_ARRAY_BUFFER, app.appUBO->bufferId);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(time), &time);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        updateInput();
        drawframe();
        
        { // rigidbody physics
            
            { // input
                if (glfwGetKey(app.window, GLFW_KEY_G)) {
                    // quat t;
                    // quatFromAxisAngle(&(vec3) {0, 1, 0}, 0.01f, &t);
                    // rbAddTorque(&boatRb, t);
                    
                    // rbAddForceAtLocation(&boatRb, (vec3) { 0, 1.0f * app.deltatime, 0 }, (vec3) { 0, 0, 10.0f });
                    // rbAddForceAtLocation(&boatRb, (vec3) {0, 0.1f, 0}, (vec3) {0, 0, -10});
                    
                    quat rot;
                    quatFromAxisAngle(&(vec3) { 0, 1, 0 }, app.deltatime * 10.0f, &rot);
                    rbAddTorque(&boatRb, rot);
                }
                
                if (glfwGetKey(app.window, GLFW_KEY_T)) {
                    boatObject.transform.position.z += 1;
                }
            }
            
            // gravity
            //rbAddForce(&boatRb, (vec3){ 0, -app.deltatime, 0} );
            boatRb.velocity.y -= 10 * app.deltatime;
            
            // bouancy
            f32 water = -3 + waterHeight(smoothBoateObject.transform.position.x, smoothBoateObject.transform.position.z);
            if (smoothBoateObject.transform.position.y < water) {
                f32 dist = smoothBoateObject.transform.position.y - water;
                rbAddForce(&boatRb, (vec3) {0, (-dist * app.deltatime * 10), 0} );
            }
            
            // air friction
            vec3Scale(&boatRb.velocity, 0.95f);
            
            // angular dampning
            vec3 axis;
            f32 angle;
            quatToAxisAngle(&boatRb.rotational_velocity, &angle, &axis);
            angle *= 0.99f;
            quatFromAxisAngle(&axis, angle, &boatRb.rotational_velocity);
            
            
            rbApplyToTransform(&boatRb, &smoothBoateObject.transform);
        }
        
        
        glfwSwapBuffers(app.window);
        glfwPollEvents();
    }
    
    glfwDestroyWindow(app.window);
    glfwTerminate();
}

