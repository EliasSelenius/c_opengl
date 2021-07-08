
#include "shader.h"

const char* vert = 
    "#version 330 core\n in vec2 a_Pos;\n void main() {\n gl_Position = vec4(a_Pos, 0.0, 1.0);\n }\n";

const char* frag =
    "#version 330 core\n out vec4 FragColor;\n void main() {\n FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n }\n";

u32 init_shader() {
    GLuint v = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(v, )
}