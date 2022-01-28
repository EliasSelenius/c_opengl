

layout(binding = 0) uniform sampler2D gBuffer_Pos;
layout(binding = 1) uniform sampler2D gBuffer_Normal;
layout(binding = 2) uniform sampler2D gBuffer_Albedo;

struct gBufferData {
    vec3 pos;
    vec3 normal;
    vec3 albedo;
    float metallic;
    float roughness;
};

gBufferData gBufferRead(vec2 uv) {
    gBufferData data;
    data.pos = texture(gBuffer_Pos, uv).xyz;
    data.normal = texture(gBuffer_Normal, uv).xyz;
    data.albedo = texture(gBuffer_Albedo, uv).rgb;

    // TODO: implement these in gBuffer
    data.metallic = 0.2;
    data.roughness = 0.1;

    return data;
}