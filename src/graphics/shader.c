#include "shader.h"
#include "UBO.h"
#include "../fileIO.h"

#include <GL.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>


static u32 makeShader(u32 program, GLenum type, const char* code, i32 codeLength) {
    u32 s = glCreateShader(type);
    glShaderSource(s, 1, &code, &codeLength); 
    glAttachShader(program, s);
    return s;
}

static void bindUBOs(u32 program) {
    i32 numUBOs, maxNameLen; 
    glGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCKS, &numUBOs);
    glGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH, &maxNameLen);

    for (int i = 0; i < numUBOs; i++) {
        char name[maxNameLen];
        i32 nameLen;
        glGetActiveUniformBlockName(program, i, maxNameLen, &nameLen, name);

        u32 uboIndex = glGetUniformBlockIndex(program, name);

        Ublock* ub = ublockGetByName(name);
        glUniformBlockBinding(program, uboIndex, ub->bindingPoint);
    }
}

u32 shaderCreate(const char* vert, i32 vertLength, 
                 const char* frag, i32 fragLength,
                 const char* geom, i32 geomLength) {
    /*
        create program
        create shaders
        compile
        attach
        link
        detach
        check for error

    */



    u32 program = glCreateProgram();

    u32 v = makeShader(program, GL_VERTEX_SHADER, vert, vertLength);
    u32 f = makeShader(program, GL_FRAGMENT_SHADER, frag, fragLength);
    u32 g = geom == NULL ? 0 : makeShader(program, GL_GEOMETRY_SHADER, geom, geomLength);

    glLinkProgram(program);

    glDetachShader(program, v);
    glDeleteShader(v);

    glDetachShader(program, f);
    glDeleteShader(f);

    if (g != 0) {
        glDetachShader(program, g);
        glDeleteShader(g);
    }

    i32 status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);

    if (status == 0) {
        char infoLog[1024];
        i32 len;
        glGetProgramInfoLog(program, sizeof(infoLog), &len, infoLog);
        printf("%s", infoLog);

        //printf("Vertex source code:\n%s\n\nFragment source code:\n%s\n\n", vert, frag);

        return 0;
    }

    bindUBOs(program);

    return program;
}

static void resolveShaderIncludes(char* code) {

}

u32 shaderLoad(const char* name) {
    
    char vertFilename[256];
    strcpy_s(vertFilename, 256, "src/graphics/shaders/");
    strcat_s(vertFilename, 256, name);

    char fragFilename[256];
    strcpy_s(fragFilename, 256, vertFilename);

    char geomFilename[256];
    strcpy_s(geomFilename, 256, vertFilename);

    strcat_s(vertFilename, 256, ".vert");
    strcat_s(fragFilename, 256, ".frag");
    strcat_s(geomFilename, 256, ".geom");
    

    //printf("%s, %s\n", vertFilename, fragFilename);

    u32 vlen, flen, glen;
    char* vert = fileread(vertFilename, &vlen);
    char* frag = fileread(fragFilename, &flen);
    char* geom = fileExists(geomFilename) ? fileread(geomFilename, &glen) : NULL;


    u32 shader = shaderCreate(vert, vlen, frag, flen, geom, glen);
    
    free(vert);
    free(frag);
    if (geom) free(geom);

    return shader;
}