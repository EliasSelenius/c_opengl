#version 330 core

// Application UBO
layout (std140) uniform Application {
    vec4 time_delta_width_height;
} app;

float getTime() {
    return app.time_delta_width_height.x;
}

vec2 getViewportSize() {
    return app.time_delta_width_height.zw;
}

// END Application UBO


layout (std140) uniform Camera {
    mat4 view;
    mat4 projection;
} camera;

uniform sampler2D u_depthTexture;
uniform float u_depthVisibility = 3.0;

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




    vec3 waterColor = vec3(0.2, 0.5, 0.8);

    vec4 shallowColor = vec4(waterColor + vec3(0.1), 0.5);
    vec4 deepColor = vec4(waterColor - vec3(0.1), 1.0);

    vec2 uv = gl_FragCoord.xy / getViewportSize();
    
    float geometryDepth = texture(u_depthTexture, uv).x;

    float waterDepth = -frag.fragpos.z;

    float depth = geometryDepth - waterDepth;
    depth = clamp(depth, 0.0, u_depthVisibility) / u_depthVisibility;
    
    vec4 finalColor = mix(shallowColor, deepColor, depth) + vec4(step(depth, 0.01));
    
    // FragColor = vec4(waterColor, 1.0);
    FragColor = vec4(finalColor.xyz * lightScale, finalColor.w);
    // FragColor = vec4(vec3(depth), 1.0);


    FragColor.rgb = pow(FragColor.rgb, vec3(1.0/2.2));

}
