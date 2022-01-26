#include "shader.h"
#include "UBO.h"
#include "../fileIO.h"
#include "../String.h"

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

static b8 checkStatus(u32 program) {
    i32 status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);

    if (status == 0) {
        char infoLog[1024];
        i32 len;
        glGetProgramInfoLog(program, sizeof(infoLog), &len, infoLog);
        printf("%s", infoLog);

        //printf("Vertex source code:\n%s\n\nFragment source code:\n%s\n\n", vert, frag);

        return false;
    }

    return true;
}

// TODO: remove ugly length parameters
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

    if (!checkStatus(program)) {
        printf("Failed to link shader.\n");
        return 0;
    }

    bindUBOs(program);

    return program;
}

u32 shaderCreateCompute(const char* src) {
    u32 program = glCreateProgram();
    u32 s = makeShader(program, GL_COMPUTE_SHADER, src, strlen(src));

    glLinkProgram(program);
    
    glDetachShader(program, s);
    glDeleteShader(s);

    if (!checkStatus(program)) return 0; // fail

    bindUBOs(program);

    return program;
}

void shaderLoadSource(StringBuilder* sb, const char* filename) {

    char filepath[256];
    strcpy_s(filepath, sizeof(filepath), "src/graphics/shaders/");
    strcat_s(filepath, sizeof(filepath), filename);

    FILE* file = fopen(filepath, "r");
    
    if (!file) {
        printf("Failed to load shader: %s\n", filename);
        return;
    } 

    u32 lineNum = 0;
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        lineNum++;
        char* s;
        if ((s = stringStartsWith(line, "#include \""))) {
            stringTrimEnd(s, '\n');
            stringTrimEnd(s, '\"');
            
            char str[16];
            sprintf(str, "#line %d\n", 1);
            sbAppend(sb, str);

            shaderLoadSource(sb, s);

            sprintf(str, "\n#line %d\n", lineNum + 1);
            sbAppend(sb, str);

        } else {
            sbAppend(sb, line);
        }
    }

    fclose(file);
}

// TODO: this could be shaderLoadByCommonFileName
// shaderLoad should take individual filenames for vert, frag and geom
u32 shaderLoad(const char* name) {
    
    char vertFilename[256];
    strcpy_s(vertFilename, sizeof(vertFilename), name);
    strcat_s(vertFilename, sizeof(vertFilename), ".vert");

    char fragFilename[256];
    strcpy_s(fragFilename, sizeof(fragFilename), name);
    strcat_s(fragFilename, sizeof(fragFilename), ".frag");

    char geomFilename[256];
    strcpy_s(geomFilename, sizeof(geomFilename), name);
    strcat_s(geomFilename, sizeof(geomFilename), ".geom");


    StringBuilder vertSb; sbInit(&vertSb);
    shaderLoadSource(&vertSb, vertFilename);

    StringBuilder fragSb; sbInit(&fragSb);
    shaderLoadSource(&fragSb, fragFilename);

    b8 hasGeometryShader = fileExists(geomFilename);
    StringBuilder geomSb;
    geomSb.content = NULL;
    geomSb.length = 0;
    if (hasGeometryShader) {
        sbInit(&geomSb);
        shaderLoadSource(&geomSb, geomFilename);
    }


    u32 shader = shaderCreate(vertSb.content, vertSb.length, 
                              fragSb.content, fragSb.length, 
                              geomSb.content, geomSb.length);

    sbDestroy(&vertSb);
    sbDestroy(&fragSb);
    if (hasGeometryShader) sbDestroy(&geomSb);

    return shader;
}


u32 shaderLoadCompute(const char* name) {
    StringBuilder sb;
    sbInit(&sb);
    shaderLoadSource(&sb, name);
    u32 p = shaderCreateCompute(sb.content);
    sbDestroy(&sb);
    return p;    
}
