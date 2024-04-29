#version 420

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D SceneTexture;

void main() {
    // Fetch the color from the scene texture
    FragColor = texture(SceneTexture, TexCoords);
}
