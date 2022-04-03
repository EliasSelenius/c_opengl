#include <GL.h>
#include "Application.h"
#include "prelude.h"
#include "fileIO.h"
#include "String.h"
#include "List.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "math/matrix.h"
#include "math/vec.h"
#include "math/quat.h"
#include "math/general.h"

#include "graphics/Mesh.h"
#include "graphics/shader.h"
#include "graphics/glUtils.h"
#include "graphics/Camera.h"
#include "graphics/obj.h"

#include "Physics.h"

#include <assert.h>

static char* g_TimerName = null;
static f64 g_TimerStart = 0.0;
static void startTimer(char* name) {
    if (g_TimerName) return;
    g_TimerName = name;
    g_TimerStart = glfwGetTime();
}
static void endTimer() {
    f64 now = glfwGetTime();

    if (!g_TimerName) return;

    f64 elapsed = now - g_TimerStart;
    printf("Timer: %s, %fms\n", g_TimerName, elapsed * 1000.0);
    g_TimerName = null;
}


Application app;

Camera g_Camera;
b8 camMode = true;

f64 mouse_x, mouse_y, pmouse_x, pmouse_y, dmouse_x, dmouse_y, mouse_scroll;
vec2 wasd;

vec3 g_SunDirection = { 1, 3, 1 };

static Mesh g_waterPlane;

static u32 g_WavesUBO;
static vec4 g_Waves[3] = {
    { 1, 1,     0.25f,   60 },
    { 1, 0.6,   0.25f,   31 },
    { 1, 1.3,   0.25f,   18 }
};

static vec3 gerstnerWave(vec2 coord, vec2 waveDir, float waveSteepness, float waveLength) {
    float k = 2.0 * PI / waveLength;
    float c = sqrt(9.8 / k);
    vec2 d = waveDir;
    vec2Normalize(&d);
    float f = k * (vec2Dot(d, coord) - c * app.time);
    float a = waveSteepness / k;

    return (vec3) {
        d.x * cos(f) * a,
              sin(f) * a,
        d.y * cos(f) * a
    };

}

static vec3 wave(vec2 coord) {
    vec3 res = {0};
    // vec3Add(&res, gerstnerWave(coord, (vec2) {1, 1}, 0.25f, 60));
    // vec3Add(&res, gerstnerWave(coord, (vec2) {1, 0.6}, 0.25f, 31));
    // vec3Add(&res, gerstnerWave(coord, (vec2) {1, 1.3}, 0.25f, 18));

    for (u32 i = 0; i < 3; i++) {
        vec3Add(&res, gerstnerWave(coord, 
            g_Waves[i].xy,
            g_Waves[i].z,
            g_Waves[i].w
        ));
    }
    
    return res;
}

static f32 approximateWaveHeight(f32 xcoord, f32 ycoord) {
    vec2 coord = {xcoord, ycoord};
    vec2 samplePoint = coord;
    vec3 point;
    
    for (u32 i = 0; i < 4; i++) {
        point = wave(samplePoint);
        samplePoint.x = coord.x - point.x;
        samplePoint.y = coord.y - point.z;
    }
    

    return wave(samplePoint).y;
}


typedef struct GizmoPoint {
    vec3 pos;
    vec3 color;
} GizmoPoint;

static vec3 gizmoCurrentColor = {1, 1, 1};
static GizmoPoint* gizmoPointsBatch;
static u32 gizmoPointsVBO;

static GizmoPoint* gizmoLinesBatch;

static void gizmoSetup() {
    gizmoPointsBatch = listCreate(GizmoPoint);
    glGenBuffers(1, &gizmoPointsVBO);

    glEnable(GL_PROGRAM_POINT_SIZE);
    // glPointSize(10);

    // glEnable(GL_LINE_SMOOTH);
    // glLineWidth(10);

}

static u32 g_RenderGizmos = true;
static void gizmoDispatch() {

    if (g_RenderGizmos) {

        glBindBuffer(GL_ARRAY_BUFFER, gizmoPointsVBO);
        u32 len = listLength(gizmoPointsBatch);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GizmoPoint) * len, gizmoPointsBatch, GL_STATIC_DRAW);
        
        glUseProgram(app.gizmoShader);

        GizmoPoint* gp = 0;

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(GizmoPoint), &gp->pos);
        
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(GizmoPoint), &gp->color);
        
        glDrawArrays(GL_POINTS, 0, len);

    }

    listClear(gizmoPointsBatch);
}

void gizmoColor(f32 red, f32 green, f32 blue) {
    gizmoCurrentColor = (vec3) {red, green, blue};
}

void gizmoPoint(vec3 pos) {
    GizmoPoint gp = {
        .pos = pos,
        .color = gizmoCurrentColor
    };
    listAdd(gizmoPointsBatch, gp);
}

void gizmoLine(vec3 start, vec3 end) {

}




typedef struct ShipType {
    Mesh mesh;

    vec3* buoyantPoints;
    u32 buoyantPointsLength;

} ShipType;


typedef struct Sail {
    Mesh mesh;
    vec3 pos;
} Sail;

typedef struct Ship {

    //ShipType* shipType;
    Transform transform;
    Rigidbody rb;
    Mesh mesh;

    u32 sailsUnfurled;
    u32 sailsCount;
    Sail* sails;    

    // NOTE: model matrix is always orthonormalized, as we only use position and rotation
    mat4 modelMatrix;
} Ship;

static Ship* g_Ships; // darray
static Ship* g_PlayerShip;

static void updateShip(Ship* ship) {

    rbUpdate(&ship->rb, &ship->transform);
    transformToMatrix(&ship->transform, &ship->modelMatrix);

    // gravity
    ship->rb.velocity.y -= 9.8f * app.deltatime;

    // render pivot point
    gizmoColor(0, 1, 0);
    gizmoPoint(ship->transform.position);

    { // buoyancy
        // TODO: we don't need floating points for each side of the boat, boats are always symmetrical
        static vec3 offsets[] = {
            { 2.1f,  -1.2f, 13.0f },
            { -2.1f, -1.2f, 13.0f },
            { 2.9f,  -1.2f, 0.0f },
            { -2.9f, -1.2f, 0.0f },
            { 2.1f,  -1.2f, -10.0f },
            { -2.1f, -1.2f, -10.0f }
        };

        i32 offsetsCount = sizeof(offsets) / sizeof(*offsets);

        for (u32 i = 0; i < offsetsCount; i++) {
            
            vec3 o = offsets[i];
            mat4MulVec3(&o, &ship->modelMatrix);
            f32 water = approximateWaveHeight(o.x, o.z);
            f32 dist = water - o.y;
            
            { // gizmo
                gizmoColor(0.8f, 0.8f, 0.8f);
                gizmoPoint(o);
                                
                vec3 w = o;
                w.y += dist;
                gizmoColor(0, 0, 1);
                gizmoPoint(w);
            }
            
            if (o.y < water) {

                vec3 offset = offsets[i];
                { // make offset be local to rb rotation
                    mat4 m = ship->modelMatrix;
                    offset = (vec3) {
                        offset.x * m.row1.x   +   offset.y * m.row2.x   +   offset.z * m.row3.x,
                        offset.x * m.row1.y   +   offset.y * m.row2.y   +   offset.z * m.row3.y,
                        offset.x * m.row1.z   +   offset.y * m.row2.z   +   offset.z * m.row3.z
                    };	
                }

                vec3 force = (vec3) { 0, dist * 2.5f * app.deltatime, 0 };
                rbAddForceAtLocation(&ship->rb, force, offset);	
            }
        }    
    }

    { // dampning
        const float dampning = 1.0f;

        vec3 f = ship->rb.velocity;
        f32 factor = clamp(0, 1, dampning * app.deltatime);
        f.x = -f.x * factor;
        f.y = -f.y * factor;
        f.z = -f.z * factor;

        vec3Add(&ship->rb.velocity, f);
    }

    { // angular dampning
        const float dampning = 10.0f;

        vec3 f = ship->rb.angularVelocity;
        f32 factor = clamp(0, 1, dampning * app.deltatime);
        f.x = -f.x * factor;
        f.y = -f.y * factor;
        f.z = -f.z * factor;

        vec3Add(&ship->rb.angularVelocity, f);
    }

    if (ship->sailsUnfurled) {
        vec3 force = ship->modelMatrix.forward;
        force = v3scale(force, app.deltatime * 15.0);
        rbAddForce(&ship->rb, force);
    }
}


static void bufferViewportSizeToUBO(f32 res[2]) {
    glBindBuffer(GL_ARRAY_BUFFER, app.appUBO->bufferId);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(f32) * 2, sizeof(f32) * 2, res);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    printf("resize: %d * %d\n", width, height);
    
    if (width < 1 || height < 1) {
        return;
    }

    app.width = width;
    app.height = height;
    
    glViewport(0, 0, width, height);
    framebufferResize(app.fbo, width, height);
    framebufferResize(app.gBuffer, width, height);
    framebufferResize(app.hdrBuffer, width, height);

    bufferViewportSizeToUBO((f32[2]){width, height});
}

void scroll_callback(GLFWwindow* window, f64 xoffset, f64 yoffset) {
    // printf("%f, %f\n", xoffset, yoffset);
    mouse_scroll = yoffset;
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

        switch (key) {
            case GLFW_KEY_F11: {
                appToggleFullscreen();
            } break;

            case GLFW_KEY_ESCAPE: {
                appExit();
            } break;

            case GLFW_KEY_F1: {
                camMode = !camMode;
            } break;

            case GLFW_KEY_R: {
                shaderReload("water", app.waterShader);
            } break;

            case GLFW_KEY_F2: {
                transformSetDefaults(&g_Camera.transform);
            } break;

            case GLFW_KEY_F3: {
                g_RenderGizmos = !g_RenderGizmos;
            } break;

            case GLFW_KEY_E: {
                g_PlayerShip->sailsUnfurled = !g_PlayerShip->sailsUnfurled;
            } break;

            case GLFW_KEY_Q: {
                static u32 shipIndex = 0;
                g_PlayerShip = &g_Ships[shipIndex++];
                if (shipIndex == listLength(g_Ships)) shipIndex = 0;
            } break;
        }
    }


}

static void initUBO(Ublock** ubo, char* name, u32 size) {
    *ubo = uboGetByName(name);
    u32 buffer = bufferCreate(NULL, size);
    uboBindBuffer(*ubo, buffer);
}

static void opengl_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
    if (type == GL_DEBUG_TYPE_ERROR) {
        printf("opengl error:\n\t %s\n\n", message);
    }
}

static u32 skyboxVAO;
static void setupSkybox() {
    vec3* vertices = (vec3[]) {
        { -1, -1, -1 },
        {  1, -1, -1 },
        { -1,  1, -1 },
        {  1,  1, -1 },
        { -1, -1,  1 },
        {  1, -1,  1 },
        { -1,  1,  1 },
        {  1,  1,  1 },
    };

    u32* indices = (u32[]) {
        0, 1, 2,
        2, 1, 3,

        1, 5, 7,
        1, 7, 3,

        0, 5, 1,
        0, 4, 5, 

        0, 2, 4,
        2, 6, 4,

        2, 3, 7,
        7, 6, 2,

        6, 5, 4,
        7, 5, 6,
    };

    u32 vao, vbo, ebo;
    vbo = bufferCreate(vertices, sizeof(vec3) * 8);
    ebo = bufferCreate(indices, sizeof(u32) * 2 * 3 * 6);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(vec3), NULL);

    glBindVertexArray(0);

    skyboxVAO = vao;
}

static void drawSkybox() {

    glUseProgram(app.skyboxShader);

    glBindVertexArray(skyboxVAO);
    glDrawElements(GL_TRIANGLES, 2 * 3 * 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
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
    glfwSetScrollCallback(app.window, scroll_callback);

    // TODO: GL_DEBUG_OUTPUT_SYNCHRONOUS
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
        app.gPassShader = shaderLoadByName("gPass");
    }

    { // light pass stuff
        FramebufferFormat attachment = fbFormat_rgb16f;                             
        app.hdrBuffer = framebufferCreate(app.width, app.height, 
                                          1, &attachment, 
                                          // giving depth to hdrBuffer for water and transparent objects
                                          fbDepth_DepthComponent);

        app.dirlightShader = shaderLoad("dirlight.frag", "scq.vert", NULL);
        app.hdr2ldrShader = shaderLoad("hdr2ldr.frag", "scq.vert", NULL);

    }
    
    
    // load shaders:
    app.waterShader = shaderLoadByName("water");
    // glUniform1ui(glGetUniformLocation(app.waterShader, "u_depthTexture"), 0);
    app.scqShader = shaderLoadByName("scq");
    // glUseProgram(app.defShader);
    app.skyboxShader = shaderLoadByName("skybox");

    app.gizmoShader = shaderLoadByName("gizmoPoint");
    gizmoSetup();

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

    { // waves UBO
        u32 ubosize = sizeof(vec4) * 3;
        g_WavesUBO = bufferCreate(g_Waves, ubosize);
        uboBindBuffer(uboGetByName("Waves"), g_WavesUBO);
    }

    { // sun UBO
        vec4 data[2];
        data[0].xyz = g_SunDirection;
        vec3Normalize(&data[0].xyz);
        data[1].xyz = (vec3) { 4, 4, 4 };

        app.sunUBO = uboGetByName("Sun");
        u32 buffer = bufferCreate(data, sizeof(data));
        uboBindBuffer(app.sunUBO, buffer);
    }

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    

    setupSkybox();

        
    // Camera
    cameraInit(&g_Camera, 3.14 / 2.0, 0.1, 1000.0);
    cameraUse(&g_Camera);
    
    { // load properties
        FILE* file;
        if (fopen_s(&file, "src/storage.txt", "r")) {
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


static void cameraBoatControll() {
    static f32 xAngle = 0.0f;
    static f32 yOffset = 10.0f;
    static f32 zoom = 10.0;

    glfwSetInputMode(app.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    g_Camera.transform.position = g_PlayerShip->transform.position;
    g_Camera.transform.position.y += yOffset;


    xAngle -= dmouse_x / 200.0f;
    yOffset = clamp(-5, 15, yOffset + dmouse_y / 10.0f);
    zoom += mouse_scroll;

    g_Camera.transform.position.x += sin(xAngle) * zoom;
    g_Camera.transform.position.z += cos(xAngle) * zoom;
    transformLookAt(&g_Camera.transform, g_PlayerShip->transform.position, (vec3) {0, 1, 0});
}

static void cameraFlyControll() {
    
    glfwSetInputMode(app.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    
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
    
    cameraUse(&g_Camera);

    { // geometry pass
        glBindFramebuffer(GL_FRAMEBUFFER, app.gBuffer->id);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);

        glUseProgram(app.gPassShader);

        u32 shipCount = listLength(g_Ships);
        for (u32 i = 0; i < shipCount; i++) {
            Ship* ship = &g_Ships[i];
            bufferInit(app.modelUBO->bufferId, &ship->modelMatrix, sizeof(mat4));
            meshRender(&ship->mesh);

            if (ship->sailsUnfurled) {
                mat4 sailMatrix = ship->modelMatrix;
                for (u32 c = 0; c < ship->sailsCount; c++) {
                    vec3 sailPos = ship->sails[c].pos;
                    sailMatrix.pos = v3add(ship->modelMatrix.pos, v3add(
                        v3scale(sailMatrix.forward, sailPos.z), 
                        v3add(
                            v3scale(sailMatrix.up, sailPos.y),
                            v3scale(sailMatrix.right, sailPos.x))));

                    bufferInit(app.modelUBO->bufferId, &sailMatrix, sizeof(mat4));
                    meshRender(&ship->sails[c].mesh);
                }
            }
        }


        { // test reading from gbuffer
            vec3 vpos;
            glReadPixels(mouse_x, app.height - mouse_y, 1, 1, GL_RGB, GL_FLOAT, &vpos);

            mat4 invView;
            mat4Transpose(&g_Camera.view, &invView);
            mat4MulVec3(&vpos, &invView);
            vpos = v3add(vpos, g_Camera.transform.position);

            gizmoColor(1, 0, 0);
            gizmoPoint(vpos);
        }
    }

    glBindFramebuffer(GL_READ_FRAMEBUFFER, app.gBuffer->id);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, app.hdrBuffer->id);
    glBlitFramebuffer(0, 0, app.width, app.height,
                      0, 0, app.width, app.height,
                      GL_DEPTH_BUFFER_BIT, GL_NEAREST);

    { // light pass
        glBindFramebuffer(GL_FRAMEBUFFER, app.hdrBuffer->id);
        glClear(GL_COLOR_BUFFER_BIT);

        drawSkybox();

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
        
        glDrawArrays(GL_TRIANGLES, 0, 6); // draw screen covering quad

    }

    { // water
        glUseProgram(app.waterShader);

        glEnable(GL_DEPTH_TEST);

        // alpha blending
        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // draw
        // planeObject.transform.position.x = round(g_Camera.transform.position.x);
        // planeObject.transform.position.z = round(g_Camera.transform.position.z);
        glUniform2f(glGetUniformLocation(app.waterShader, "u_worldPos"), 
            round(g_Camera.transform.position.x),
            round(g_Camera.transform.position.z));

        meshRender(&g_waterPlane);
    }

    { // draw to screen (hdr -> ldr)
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, app.hdrBuffer->attachments[0].texture);
        
        glUseProgram(app.hdr2ldrShader);
        
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }


    gizmoDispatch();

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

// TODO: move this to obj file
static OBJ* getObjByIndex(OBJ* obj, u32 index) {
    while (index) {
        obj = obj->next;
        index--;
    }
    return obj;
}
static u32 objChildCount(OBJ* obj) {
    u32 i = 0;
    obj = obj->child;
    while (obj) {
        i++;
        obj = obj->next;
    }
    return i;
}


static void objGetMesh(OBJ* obj, Mesh* mesh) {
    MeshData data;
    objToSmoothShadedMesh(obj, &data);
    meshFromData(&data, mesh);

    free(data.vertices);
    free(data.indices);
}


int main() {

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
        }

        meshGenNormals(&planeData);

        Mesh islandMesh;
        meshFromData(&planeData, &islandMesh);

    }


    /*    
    { // pyramid
        OBJ* obj = objLoad("src/models/pyramid.obj");
        
        MeshData objData;
        objToFlatShadedMesh(obj, &objData);
        
        objFree(obj);
        
        Mesh m;
        meshCreate(objData.vertexCount, objData.vertices, objData.indexCount, objData.indices, &m);
    } */ 
    
    { // load ships
        g_Ships = listCreate(Ship);

        OBJ* obj = objLoad("src/models/Ships.obj");

        u32 i = 0;

        OBJ* cobj = obj;
        while (cobj) {
            
            
            Ship ship = {0};
            ship.rb.mass = 1;
            transformSetDefaults(&ship.transform);
            ship.transform.position = cobj->position;
            transformToMatrix(&ship.transform, &ship.modelMatrix);

            objGetMesh(cobj, &ship.mesh);

            ship.sailsUnfurled = false;
            ship.sailsCount = objChildCount(cobj);
            ship.sails = malloc(sizeof(Sail) * ship.sailsCount);
            u32 childIndex = 0;
            OBJ* child = cobj->child;
            while (child) {
                ship.sails[childIndex].pos = child->position;
                objGetMesh(child, &ship.sails[childIndex].mesh);
                child = child->next;
                childIndex++;
            }

            listAdd(g_Ships, ship);

            i++;

            cobj = cobj->next;
        }

        objFree(obj);
    }

    { // boat

        Ship ship = {0};
        ship.rb.mass = 1.0f;

        transformSetDefaults(&ship.transform);
        ship.transform.position = (vec3) { 40, 4, 0 };
        transformToMatrix(&ship.transform, &ship.modelMatrix);

        OBJ* obj = objLoad("src/models/Boate.obj");
        objGetMesh(obj, &ship.mesh);
        
        u32 childCount = objChildCount(obj);
        ship.sailsUnfurled = false;
        ship.sails = malloc(sizeof(Sail) * childCount);
        ship.sailsCount = childCount;
        OBJ* child = obj->child;
        u32 i = 0;
        while (child) {
            ship.sails[i].pos = child->position;
            objGetMesh(child, &ship.sails[i].mesh);
            child = child->next;
            i++;
        }
        
        objFree(obj);

        listAdd(g_Ships, ship);
        g_PlayerShip = &g_Ships[listLength(g_Ships) - 10];
    }

    
    { // water
        MeshData planeData;
        genPlane(&planeData, 1000);
        meshFromData(&planeData, &g_waterPlane);

        free(planeData.indices);
        free(planeData.vertices);
    }
    

    static f64 targetdelta = 1.0 / 60.0;
    app.time = glfwGetTime();
    app.prevtime = app.time;
    printf("starting time: %f\n", app.time);


    while (!glfwWindowShouldClose(app.window)) {

        // calc time
        app.prevtime = app.time;
        app.time = glfwGetTime();
        app.deltatime = app.time - app.prevtime;
        
        // is deltatime unreasonably large? if so pretend time did'nt move
        if (app.deltatime > targetdelta * 2) {
            static u32 skips = 0;
            printf("%d. recalc deltatime, from %fms\n", ++skips, app.deltatime * 1000);

            app.deltatime = targetdelta;
            app.time = app.prevtime + targetdelta;
            glfwSetTime(app.time);
        }
        
        // tell the GPU about time 
        f32 time = app.time;
        glBindBuffer(GL_ARRAY_BUFFER, app.appUBO->bufferId);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(time), &time);
        glBindBuffer(GL_ARRAY_BUFFER, 0);


        updateInput();
        

        { // render wave points
            for (u32 x = 0; x < 20; x++) {
                for (u32 z = 0; z < 20; z++) {
                    float r = random2(x, z);
                    r = (r + 1.0f) / 2.0f;
                    gizmoColor(r, r, r);

                    vec2 coord = {x, z};
                    f32 h = approximateWaveHeight(coord.x, coord.y);
                    gizmoPoint((vec3) {x, h, z});
                }
            }
        }

        if (true) {
            u32 len = listLength(g_Ships);
            for (u32 i = 0; i < len; i++) {
                updateShip(&g_Ships[i]);
            }
        }

        if (camMode) {
            cameraFlyControll();
        } else {
            cameraBoatControll();
            if (wasd.y < 0) rbAddForce(&g_PlayerShip->rb, (vec3) {0, 1, 0});            
            vec3Add(&g_PlayerShip->rb.angularVelocity, (vec3) { 0, -wasd.x * app.deltatime * 5.0, 0 });
        }
                
        drawframe();
        mouse_scroll = 0.0f;

        glfwSwapBuffers(app.window);
        glfwPollEvents();
    }

    
    { // save properties
        FILE* file;
        if (fopen_s(&file, "src/storage.txt", "w")) {
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