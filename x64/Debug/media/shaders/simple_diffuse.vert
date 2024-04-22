#version 430 core

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
uniform mat4 view;
uniform mat4 projection;
uniform vec3 cameraPos;

void main() {
    TexCoords = aTexCoords;
    LightMapTexCoords = aLightMapTexCoords;
    
    WorldPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;

    vec3 worldNormal = normalize(mat3(model) * aNormal);
    vec3 worldViewDir = normalize(WorldPos - cameraPos);
    ReflectDir = reflect(-worldViewDir, worldNormal);

    gl_Position = projection * view * vec4(WorldPos, 1.0);
}