#version 430 core

layout (std140, binding = 0) uniform MatrixBlock {
    mat4 view;
    mat4 projection;
} Matrices;

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
    gl_Position = Matrices.projection * Matrices.view * model * vec4(aPos, 1.0);
}
