#include "shader.h"
#include "UBO.h"

#include <GL.h>

#include <string.h>
#include <stdio.h>

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

u32 shaderCreate(const char* vert, i32 vertLength, const char* frag, i32 fragLength) {
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

    glLinkProgram(program);

    glDetachShader(program, v);
    glDeleteShader(v);

    glDetachShader(program, f);
    glDeleteShader(f);

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


