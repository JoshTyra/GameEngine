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

in vec2 TexCoords;
in vec2 LightMapTexCoords;
in vec3 ReflectDir;
in vec3 Normal;
in vec3 WorldPos;

out vec4 FragColor;

uniform samplerCube environmentMap;

void main() {
    vec3 envColor = texture(environmentMap, ReflectDir).rgb * 0.25f;
    FragColor = vec4(envColor, 1.0);
}