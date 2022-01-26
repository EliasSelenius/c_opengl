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
    
    app.width = width;
    app.height = height;
    
    glViewport(0, 0, width, height);
    framebufferResize(app.fbo, width, height);
    framebufferResize(app.gBuffer, width, height);

    bufferViewportSizeToUBO((f32[2]){width, height});
}

static GLFWmonitor* getIdealMonitor() {

    i32 windowX, windowY, windowW, windowH;
    glfwGetWindowPos(app.window, &windowX, &windowY);
    glfwGetWindowSize(app.window, &windowW, &windowH);

    i32 monitorCount;
    GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);

    u32 ideal = 0;
    u32 highestArea = 0;
    for (u32 i = 0; i < monitorCount; i++) {
        GLFWmonitor* m = monitors[i];

        i32 mX, mY;
        glfwGetMonitorPos(m, &mX, &mY);
        const GLFWvidmode* mode = glfwGetVideoMode(m);

        // TODO: better min/max macros or inline functions
        u32 area = max(0, min(windowX + windowW, mX + mode->width) - max(windowX, mX)) *
                    max(0, min(windowY + windowH, mY + mode->height) - max(windowY, mY));

        if (area > highestArea) {
            highestArea = area;
            ideal = i;
        }

        // printf("Monitor %d. %s\n    Area: %d\n", i, glfwGetMonitorName(m), area);
    }

    return monitors[ideal];
}

void appToggleFullscreen() {
    GLFWmonitor* monitor = glfwGetWindowMonitor(app.window);
    if (monitor) {

        // set to windowed 

        i32 mX, mY;
        glfwGetMonitorPos(monitor, &mX, &mY);

        glfwSetWindowMonitor(app.window, NULL, mX + 60, mY + 60, 1600, 900, 0);

    } else {

        // set to fullscreen

        GLFWmonitor* m = getIdealMonitor();   
        const GLFWvidmode* mode = glfwGetVideoMode(m);
        glfwSetWindowMonitor(app.window, m, 0, 0, mode->width, mode->height, mode->refreshRate);
    }        
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // printf("%i, %i, %i, %i\n", key, scancode, action, mods);

    if (action == GLFW_RELEASE) {

        if (key == GLFW_KEY_F11) {
            appToggleFullscreen();
        } else if (key == GLFW_KEY_ESCAPE) {
            appExit();
        }
    }


}

static void initUBO(Ublock** ubo, char* name, u32 size) {
    *ubo = ublockGetByName(name);
    u32 buffer = bufferCreate(NULL, size);
    ublockBindBuffer(*ubo, buffer);
}

static void opengl_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
    // printf("opengl error:\n\t %s\n\n", message);
}

int appInit() {
    
    if (!glfwInit())
        return -1;
    
    
	app.width = 1600;
    app.height = 900;
    app.window = glfwCreateWindow(app.width, app.height, "Test window", NULL, NULL);
    if (!app.window) {
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(app.window);
    gladLoadGL(glfwGetProcAddress);
    
    glfwSetFramebufferSizeCallback(app.window, framebuffer_size_callback);
    glfwSetKeyCallback(app.window, key_callback);
    
    // GL_DEBUG_OUTPUT_SYNCHRONOUS
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(&opengl_debug_callback, NULL);

    { // geometry pass stuff
        const u32 attachCount = 3;
        FramebufferFormat attachments[attachCount] = {
            fbFormat_rgb16f, // position
            fbFormat_rgb16f, // normal
            fbFormat_rgb8    // albedo

            // TODO: PBR properties
        };

        app.gBuffer = framebufferCreate(app.width, app.height, attachCount, attachments, fbDepth_DepthComponent);
        app.gPassShader = shaderLoad("gPass");        
    }

    { // light pass stuff
        FramebufferFormat attachment = fbFormat_rgba16f;
        app.hdrBuffer = framebufferCreate(app.width, app.height, 1, &attachment, fbDepth_None);

        { // manually load shader. TODO: Maybe we replace this with a function 
            StringBuilder sbVert;
            sbInit(&sbVert);
            shaderLoadSource(&sbVert, "scq.vert");

            StringBuilder sbFrag;
            sbInit(&sbFrag);
            shaderLoadSource(&sbFrag, "dirlight.frag");

            app.dirlightShader = shaderCreate(sbVert.content, sbVert.length,
                                              sbFrag.content, sbFrag.length,
                                              NULL, 0);

            sbDestroy(&sbVert);
            sbDestroy(&sbFrag);

            glUseProgram(app.dirlightShader);
            // glUniform1i(glGetUniformLocation(app.dirlightShader, "gBuffer_Pos"), 0);
            // glUniform1i(glGetUniformLocation(app.dirlightShader, "gBuffer_Normal"), 1);
            // glUniform1i(glGetUniformLocation(app.dirlightShader, "gBuffer_Albedo"), 2);
        }





    }
    
    
    // load shaders:
    // app.defShader = shaderLoad("def");
    // app.waterShader = shaderLoad("water");
    // glUniform1ui(glGetUniformLocation(app.waterShader, "u_depthTexture"), 0);
    app.scqShader = shaderLoad("scq");
    // glUseProgram(app.defShader);
    
    FramebufferFormat attachments[] = {
        fbFormat_rgba8,
        fbFormat_float16
    };
    
    i32 w, h;
    glfwGetFramebufferSize(app.window, &w, &h);
    app.fbo = framebufferCreate(w, h, 2, attachments, fbDepth_DepthComponent);
    
    
    initUBO(&app.cameraUBO, "Camera", sizeof(mat4) * 2);
    initUBO(&app.modelUBO, "Model", sizeof(mat4));
    initUBO(&app.appUBO, "Application", sizeof(f32) * 4);
    bufferViewportSizeToUBO((f32[2]){w,h});
    
    
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    // TODO look up glMultiDrawElements()
    
    
    sceneInit(&scene);
    
    // Camera
    cameraInit(&g_Camera, 3.14 / 2.0, 0.1, 1000.0);
    cameraUse(&g_Camera);
    
    { // load properties
        FILE* file = fopen("src/storage.txt", "r");

        if (file == NULL) {
            printf("could not read storage.txt\n");
        } else {
            char line[256];
            fgets(line, sizeof(line), file);

            char* cur = line;

            g_Camera.transform.position.x = strtof(cur, &cur);
            g_Camera.transform.position.y = strtof(cur, &cur);
            g_Camera.transform.position.z = strtof(cur, &cur);

            g_Camera.transform.rotation.x = strtof(cur, &cur);
            g_Camera.transform.rotation.y = strtof(cur, &cur);
            g_Camera.transform.rotation.z = strtof(cur, &cur);
            g_Camera.transform.rotation.w = strtof(cur, &cur);

            fclose(file);
        }
    }

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
    
    
    
    f32 scale = 3.0f;
    if (glfwGetKey(app.window, GLFW_KEY_LEFT_SHIFT)) {
        scale = 10.0f;
    }
    
    vec3Scale(&cam_model.row3.xyz, wasd.y / 10.0f * scale);
    vec3Scale(&cam_model.row1.xyz, wasd.x / 10.0f * scale);
    
    vec3Add(&g_Camera.transform.position, cam_model.row3.xyz);
    vec3Add(&g_Camera.transform.position, cam_model.row1.xyz);
    
    
}

static void drawframe() {
    glClearColor(0,0,0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    
    cameraFlyControll();
    cameraUse(&g_Camera);


    { // geometry pass
        glBindFramebuffer(GL_FRAMEBUFFER, app.gBuffer->id);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);

        glUseProgram(app.gPassShader);

        sceneRender(&scene);
    }

    { // light pass
        glBindFramebuffer(GL_FRAMEBUFFER, app.hdrBuffer->id);
        glClear(GL_COLOR_BUFFER_BIT);

        glDisable(GL_DEPTH_TEST);
        
        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_ONE, GL_ONE);

        glUseProgram(app.dirlightShader);

        // bind gBuffer for reading
         
        for (u32 i = 0; i < app.gBuffer->attachmentCount; i++) {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, app.gBuffer->attachments[i].texture);
        } 
        

        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, app.gBuffer->attachments[0].texture);

        glDrawArrays(GL_TRIANGLES, 0, 6); // draw screen covering quad
    }

    { // draw to screen
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, app.hdrBuffer->attachments[0].texture);
        glUseProgram(app.scqShader);
        glDisable(GL_DEPTH_TEST);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }


    //  OLD  // 
    /*

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
    */    
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

    if (false) { // test math funcs
        printf("lerp: 3, 6 = %f\n", lerp(3, 6, 0.5f));

        printf("fract = %f\n", fract(4.512f));
    }


    if (false) { // test shader includes
        StringBuilder sb;
        sbInit(&sb);

        shaderLoadSource(&sb, "def.frag");

        printf("%s", sb.content);
    }


    if (false) { // test string view
        Strview sv = svFrom("  \t\t Hello There!   \n");
        sv = svTrim(sv);

        printf("Start with: %d\n", svStartsWith(svFrom("Hello dwajdoaw"), "Hello d"));

        char str[256];
        svCopyTo(sv, str);

        str[sv.length] = '\0';
        printf("TEST: |%s|\n", str);
    }

    if (false) { // test stringBuilder
        StringBuilder sb;
        sbInit(&sb);
        sbAppendView(&sb, svFrom("Hello"));
        sbAppend(&sb, " ");
        sbAppendView(&sb, svTrim(svFrom("\nWorld   ")));
        sbAppendView(&sb, svFrom("!"));
        sbAppend(&sb, "\n");
        sbAppendView(&sb, svFrom("Cool"));
        sbAppendView(&sb, svFrom(" "));
        sbAppend(&sb, "String Builder");
        sbAppendView(&sb, svFrom(" dude!"));

        char buffer[256] = {};
        sbCopyIntoBuffer(&sb, buffer, 256);
        //buffer[5] = '\0';
        printf("Buffer Content:\n%s\n", buffer);

        printf("Builder:\n    Capacity: %d\n    Length: %d\n    Content:\n%s\n", sb.capacity, sb.length, sb.content);

        sbDestroy(&sb);
    }

    // return 0;
    
    
    
    if (!appInit()) return -1;
    

    { // Island
        MeshData planeData;
        genPlane(&planeData, 1000);

        for (u32 i = 0; i < planeData.vertexCount; i++) {
            f32 x = planeData.vertices[i].pos.x;
            f32 z = planeData.vertices[i].pos.z;
            f32 d = x*x + z*z;
            // planeData.vertices[i].pos.y = -d / 200.0f;
            // const f32 freq = 100.0f;
            // planeData.vertices[i].pos.y += gnoise2(1000.0f + (x / freq), 1000.0f + (z / freq)) * 50.0f;


            x /= 200.0f;
            z /= 200.0f;
            f32 v = 0.0f;
            for (u32 i = 1; i <= 3; i++) {
                v += gnoise2(x * i, z * i) / (f32)i;
            }
            v *= 4.0f;
            v *= v * v;
            // v *= 100.0f;

            planeData.vertices[i].pos.y = v;

            planeData.vertices[i].pos.y -= 50.0f;


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

        // NOTE: we are copying the objects in to the scene.
        listAdd(scene.gameobjects, boatObject);
        listAdd(scene.gameobjects, smoothBoateObject);
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

        // printf("deltatime: %f\n", app.deltatime);

        glfwSwapBuffers(app.window);
        glfwPollEvents();
    }

    //glfwGetWindowPos
    
    { // save properties
        FILE* file = fopen("src/storage.txt", "w");

        if (file == NULL) {
            printf("Could not save to storage.txt");
        } else {
            fprintf(file, "%f %f %f %f %f %f %f",
                g_Camera.transform.position.x, 
                g_Camera.transform.position.y,
                g_Camera.transform.position.z,

                g_Camera.transform.rotation.x,
                g_Camera.transform.rotation.y,
                g_Camera.transform.rotation.z,
                g_Camera.transform.rotation.w);

            fclose(file);
        }
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

