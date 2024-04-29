#version 430 core

layout (std140, binding = 0) uniform Uniforms {
    mat4 view;
    mat4 projection;
    vec3 cameraPositionWorld;
    float _pad1;
    vec3 cameraPositionEyeSpace;
    float _pad2;
    vec4 lightColor;
    vec3 lightDirectionWorld;
    float _pad3;
    vec3 lightDirectionEyeSpace;
    float _pad4;
    float lightIntensity;
    float nearPlane;
    float farPlane;
    float _pad5[8]; // Increase the size of the padding array to 8 elements
};

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec2 aLightMapTexCoords;

out vec2 TexCoords;
out vec2 LightMapTexCoords;
out vec3 ReflectDir;
out vec3 Normal;
out vec3 WorldPos;

uniform mat4 model;

void main() {
    TexCoords = aTexCoords;
    LightMapTexCoords = aLightMapTexCoords;
    
    WorldPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;

    vec3 worldNormal = normalize(mat3(model) * aNormal);
    vec3 worldViewDir = normalize(WorldPos - cameraPositionWorld);
    ReflectDir = reflect(-worldViewDir, worldNormal);

    gl_Position = projection * view * vec4(WorldPos, 1.0);
}