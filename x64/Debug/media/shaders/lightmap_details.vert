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

layout (location = 0) in vec3 aPos; // Position vector
layout (location = 1) in vec3 aNormal; // Normal vector
layout (location = 2) in vec2 aTexCoords; // Base texture UVs
layout (location = 3) in vec2 aLightMapTexCoords; // Lightmap UVs

out vec2 TexCoords;
out vec2 LightMapTexCoords;

uniform mat4 model;

void main() {
    TexCoords = aTexCoords;
    LightMapTexCoords = aLightMapTexCoords;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
