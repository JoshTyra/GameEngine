#version 420 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D sceneTexture;
uniform float resolutionFactor; // How much to downsample. For half-size, use 0.5.

void main() {
    // Average 4 neighboring pixels to downsample. Adjust the offsets if you change resolutionFactor.
    vec2 texOffset = 1.0 / textureSize(sceneTexture, 0) * resolutionFactor;
    vec4 sample0 = texture(sceneTexture, TexCoords + vec2(-texOffset.x, -texOffset.y));
    vec4 sample1 = texture(sceneTexture, TexCoords + vec2(texOffset.x, -texOffset.y));
    vec4 sample2 = texture(sceneTexture, TexCoords + vec2(-texOffset.x, texOffset.y));
    vec4 sample3 = texture(sceneTexture, TexCoords + vec2(texOffset.x, texOffset.y));
    
    FragColor = (sample0 + sample1 + sample2 + sample3) * 0.25;
}
