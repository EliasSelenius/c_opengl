#version 420 core

#include "app.glsl"
#include "camera.glsl"
#include "PBR.glsl"


layout(binding = 0) uniform sampler2D gBuffer_Pos;
uniform float u_depthVisibility = 10.0;


out vec4 FragColor;

in Fragdata {
    vec3 fragpos;
    vec3 normal;
    vec4 color;
} frag;

void main() {
    //if (length(frag.fragpos) > 25) discard; 


    vec3 ld = normalize(vec3(1, 3, 1));
    ld = (camera.view * vec4(ld, 0.0)).xyz;


    vec3 normal = normalize(frag.normal);
    

    vec4 finalColor;

    { // water color
        vec3 waterColor = vec3(0.2, 0.5, 0.8);

        vec4 shallowColor = vec4(waterColor + vec3(0.1), 0.5);
        vec4 deepColor = vec4(waterColor - vec3(0.1), 1.0);

        vec2 uv = gl_FragCoord.xy / getViewportSize();
        
        // float geometryDepth = texture(u_depthTexture, uv).x;
        float geometryDepth = length(texture(gBuffer_Pos, uv).xyz);
        if (geometryDepth < 0.001) {
            geometryDepth = 9999.0;
        }

        float waterDepth = length(frag.fragpos);

        float depth = geometryDepth - waterDepth;
        depth = clamp(depth, 0.0, u_depthVisibility) / u_depthVisibility;
        
        finalColor = mix(shallowColor, deepColor, depth) + vec4(step(depth, 0.01));
    }

    const float metallic = 0.0;

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, vec3(finalColor), metallic);

    vec3 V = normalize(-frag.fragpos);

    FragColor.rgb = CalcDirlight(
        /* light direction */ ld,
        /* light color     */ vec3(4.0),
        /*                 */ F0,
        /* surface normal  */ normal,
        /*                 */ V,
        /* surface color   */ vec3(finalColor),
        /* roughness       */ 0.5,
        /* metallic        */ metallic);

    FragColor.a = finalColor.a;

}
