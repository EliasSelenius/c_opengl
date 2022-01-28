#version 430 core

uniform sampler2D hdrTexture;

in vec2 uv;
out vec3 FragColor;

void main() {
    vec3 color = texture(hdrTexture, uv).rgb;
 

    // reinhard tone mapping
    //color = color / (color + vec3(1.0));
    // alternative tone mapping with exposure:
    // TODO: make exposure adjustable    
    float exposure = 1.0;
    color = vec3(1.0) - exp(-color * exposure);


    // gamma correction
    color = pow(color, vec3(1.0 / 2.2));


    FragColor = color;
}