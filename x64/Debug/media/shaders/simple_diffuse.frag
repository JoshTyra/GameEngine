#version 430 core

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