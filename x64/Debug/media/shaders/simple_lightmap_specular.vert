#version 430 core

layout (std140, binding = 0) uniform Uniforms {
    mat4 view;
    mat4 projection;
    vec3 cameraPositionWorld;
    vec3 cameraPositionEyeSpace;
    vec4 lightColor;
    vec3 lightDirectionWorld;
    vec3 lightDirectionEyeSpace;
    float lightIntensity;
    float nearPlane;
    float farPlane;
};

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec2 aLightMapTexCoords;

out vec2 TexCoords;
out vec2 LightMapTexCoords;
out vec3 ReflectDir;

uniform mat4 model;

void main() {
    TexCoords = aTexCoords;
    LightMapTexCoords = aLightMapTexCoords;

    vec4 worldPos = model * vec4(aPos, 1.0);
    vec3 worldNormal = normalize(mat3(transpose(inverse(model))) * aNormal);

    vec3 viewPos = vec3(view * worldPos);
    vec3 viewNormal = normalize(mat3(view) * worldNormal);

    vec3 viewDir = normalize(-viewPos);
    ReflectDir = reflect(viewDir, viewNormal);

    gl_Position = projection * view * worldPos;
}