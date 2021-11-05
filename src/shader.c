#include "shader.h"

#include <GL.h>

u32 makeShader(u32 program, GLenum type, const char* code) {
    u32 s = glCreateShader(type);
    u32 len = strlen(code);
    glShaderSource(s, 1, &code, &len); 
    glAttachShader(program, s);
    return s;
}

u32 shaderCreate(const char* vert, const char* frag) {
    /*
        create program
        create shaders
        compile
        attach
        link
        dettach
        check for error

    */



    u32 program = glCreateProgram();

    u32 v = makeShader(program, GL_VERTEX_SHADER, vert);
    u32 f = makeShader(program, GL_FRAGMENT_SHADER, frag);

    glLinkProgram(program);

    glDetachShader(program, v);
    glDeleteShader(v);

    glDetachShader(program, f);
    glDeleteShader(f);

    u32 status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);

    if (status == 0) {
        char buf[255];
        i32 len;
        char* infoLog;
        glGetProgramInfoLog(program, sizeof(buf), &len, infoLog);
        printf("%s", infoLog);
    }


    return program;
}


