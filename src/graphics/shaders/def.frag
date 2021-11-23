#version 330 core

layout (std140) uniform Camera {
    mat4 view;
    mat4 projection;
} camera;

out vec4 FragColor;

in Fragdata {
    vec3 fragpos;
    vec3 normal;
    vec4 color;
} frag;

void main() {
    vec3 ld = normalize(vec3(1, 1, 1));

    vec3 lightDir = (camera.view * vec4(ld, 0.0)).xyz;


    vec3 normal = normalize(frag.normal);
    
    //FragColor = vec4(normal, 1.0);
    //FragColor = vec4(lightDir, 1.0);
    FragColor = frag.color * max(dot(normal, lightDir), 0.0);
}
