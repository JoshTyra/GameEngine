#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec2 aLightMapTexCoords;

out vec2 TexCoords;
out vec2 LightMapTexCoords;
out vec3 ReflectDir; // Output the reflection direction

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;
uniform vec3 cameraPos; // Camera position in world space
uniform mat3 normalMatrix; // Normal matrix for correct normal transformations

void main() {
    TexCoords = aTexCoords;
    LightMapTexCoords = aLightMapTexCoords;
    vec4 worldPos = model * vec4(aPos, 1.0);
    vec3 normal = normalize(normalMatrix * aNormal);
    vec3 viewDir = normalize(cameraPos - vec3(worldPos));  // Calculate view direction
    ReflectDir = reflect(viewDir, normal);  // Calculate reflection direction
    gl_Position = projection * view * worldPos;
}
