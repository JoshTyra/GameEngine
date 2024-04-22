#version 420 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D sceneTexture;

void main() {
    // The hardware automatically applies bilinear filtering when min/mag filter is set to GL_LINEAR
    FragColor = texture(sceneTexture, TexCoords);
}
