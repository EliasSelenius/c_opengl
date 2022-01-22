#version 330 core

#include "camera.glsl"

out vec4 FragColor;
out float FragDepth;

in Fragdata {
    vec3 fragpos;
    vec3 normal;
    vec4 color;
} frag;

void main() {
    // vec3 ld = normalize(vec3(sin(Time), cos(Time), 0));
    vec3 ld = normalize(vec3(1.0, 1.0, 1.0));

    vec3 lightDir = (camera.view * vec4(ld, 0.0)).xyz;


    vec3 normal = normalize(frag.normal);
    
    //FragColor = vec4(1, 1, 0, 1);
    //FragColor = vec4(normal, 1.0);
    //FragColor = vec4(lightDir, 1.0);
    FragColor = frag.color * max(dot(normal, lightDir), 0.0);
    FragDepth = length(frag.fragpos);
}
