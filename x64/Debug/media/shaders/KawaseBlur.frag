// KawaseBlur.frag
#version 420 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D sceneTexture;
uniform vec2 blurOffset;

void main() {
    vec4 color = texture(sceneTexture, TexCoords) * 0.5;
    color += texture(sceneTexture, TexCoords + vec2(blurOffset.x, 0.0)) * 0.25;
    color += texture(sceneTexture, TexCoords - vec2(blurOffset.x, 0.0)) * 0.25;
    color += texture(sceneTexture, TexCoords + vec2(0.0, blurOffset.y)) * 0.25;
    color += texture(sceneTexture, TexCoords - vec2(0.0, blurOffset.y)) * 0.25;
    FragColor = color;
}
