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

#include <assert.h>


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

void boatBouancy(Rigidbody* rb) {
    static vec3 offsets[] = {
        { 2.1f,  -0.2f, 13.0f },
        { -2.1f, -0.2f, 13.0f },
        { 2.1f,  -0.2f, -10.0f },
        { -2.1f, -0.2f, -10.0f }
    };

    mat4 m;
    transformToMatrix(rb->transform, &m);
                    
    for (u32 i = 0; i < 4; i++) {
        vec3 o = offsets[i];
        mat4MulVec3(&o, &m);
        f32 water = -3 + waterHeight(o.x, o.z);
        if (o.y < water) {
            //printf("%d is underwater\n", i);
            f32 dist = water - o.y;
            rbAddForceAtLocation(rb, (vec3) { 0, dist * 2.5f * app.deltatime, 0 }, offsets[i]);	
        }
    }    
}

void applyPhysics(Rigidbody* rb) {
            
    // gravity
    rb->velocity.y -= 10 * app.deltatime;
    
    boatBouancy(rb);				
    

    // TODO: linear and angular dampning scaled by deltatime
    // air friction
    vec3Scale(&rb->velocity, 0.95f);
    // angular dampning
    vec3Scale(&rb->angularVelocity, 0.95f);

    rbUpdate(rb);
}

#include "math/general.h"

int main() {

    { // test math funcs
        printf("lerp: 3, 6 = %f\n", lerp(3, 6, 0.5f));

        printf("fract = %f\n", fract(4.512f));
    }


    { // test shader includes
        shaderReadFromFile("src/graphics/shaders/def.frag");
    }


    { // test string view
        Strview sv = svFrom("  \t\t Hello There!   \n");
        sv = svTrim(sv);

        printf("Start with: %d\n", svStartsWith(svFrom("Hello dwajdoaw"), "Hello d"));

        char str[256];
        svCopyTo(sv, str);

        str[sv.length] = '\0';
        printf("TEST: |%s|\n", str);
    }

    { // test stringBuilder
        StringBuilder sb;
        sbInit(&sb);
        sbAppend(&sb, svFrom("Hello"));
        sbAppend(&sb, svFrom(" "));
        sbAppend(&sb, svTrim(svFrom("\nWorld   ")));
        sbAppend(&sb, svFrom("!"));

        char buffer[256] = {};
        sbCopyIntoBuffer(&sb, buffer, 256);
        //buffer[5] = '\0';
        printf("Buffer Content:\n%s\n", buffer);

        sbDestroy(&sb);
    }

    // return 0;
    
    //shaderReadFromFile("src/graphics/shaders/def.frag");
    
    
    if (!appInit()) return -1;
    

    { // Island
        MeshData planeData;
        genPlane(&planeData, 100);

        for (u32 i = 0; i < planeData.vertexCount; i++) {
            f32 x = planeData.vertices[i].pos.x;
            f32 z = planeData.vertices[i].pos.z;
            f32 d = x*x + z*z;
            planeData.vertices[i].pos.y = -d / 200.0f;
            const f32 freq = 15.0f;
            planeData.vertices[i].pos.y += gnoise2(1000.0f + (x / freq), 1000.0f + (z / freq)) * 7.0f;

            if (planeData.vertices[i].pos.y > -2) {
                planeData.vertices[i].color = (vec4) { 0.1f, 0.9f, 0.2f, 1.0f };
            } else {
                planeData.vertices[i].color = (vec4) { 1.0f, 1.0f, 101.0f / 256.0f, 1.0f };
            }
        }

        meshGenNormals(&planeData);

        Mesh islandMesh;
        meshFromData(&planeData, &islandMesh);

        Gameobject obj = {
            .mesh = &islandMesh,
            .parent = NULL,
            .transform = {
                .position = { -20, 0, 0 },
                .rotation = { 0, 0, 0, 1 },
                .scale = { 1, 1, 1 }
            }
        };

        listAdd(scene.gameobjects, obj);
    }

    
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
        
        boatObject.transform.position = (vec3) { 20, 4, 0 };
        smoothBoateObject.transform.position = (vec3) { 30, 0, 0 };
    }
    
    { // boat Rb
        boatRb = (Rigidbody) {
            .mass = 1.0f,
            //.rotational_velocity = (quat) { 0.0f, 0.0f, 0.0f, 1.0f },
			.transform = &smoothBoateObject.transform
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
    
    static f64 acTime = 0.0;

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

        acTime += app.deltatime;
        if (acTime > 0.016) {
            acTime -= 0.016;
            app.deltatime = 0.016;
            
            applyPhysics(&boatRb);
            
            static Rigidbody rb = (Rigidbody) {
                .mass = 1.0f,
                .transform = &boatObject.transform
            };

            applyPhysics(&rb);
        }

        printf("deltatime: %f\n", app.deltatime);

        glfwSwapBuffers(app.window);
        glfwPollEvents();
    }
    
    if (random((i32)app.time) < 0) {
        printf("Commencing Program Self-Destruct...\n");
    } else {
        printf("Program Committed Suicide...\n");
        // Program Was Slain By User. Game Over.
        // Program Ran Out Of Battery Power...
        // Program Was Terminated. *sad program noises*
    }

    glfwDestroyWindow(app.window);
    glfwTerminate();
}

