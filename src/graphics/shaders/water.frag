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
    //if (length(frag.fragpos) > 25) discard; 

    vec3 ld = normalize(vec3(1, 1, 1));
    vec3 lightDir = (camera.view * vec4(ld, 0.0)).xyz;

    vec3 normal = normalize(frag.normal);

    float ambientScale = 0.3;    
    float diffuseScale = max(dot(normal, lightDir), 0.0);
    float specularScale = pow(max(dot(normalize(-frag.fragpos), reflect(-lightDir, normal)), 0.0), 32);

    float lightScale = ambientScale + diffuseScale + specularScale;

    vec3 waterColor = vec3(0.2, 0.6, 0.7);

    vec4 shallowColor = vec4(waterColor + vec3(0.1), 0.4);
    vec4 deepColor = vec4(waterColor - vec3(0.1), 1.0);


    float depth = length(frag.fragpos);
    float alpha = depth / 20.0;
    alpha = clamp(alpha, 0.0, 1.0);
    vec4 finalColor = mix(shallowColor, deepColor, alpha);
    
    FragColor = vec4(finalColor.xyz * lightScale, finalColor.w);
}
