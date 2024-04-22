#version 420 core

layout (location = 0) in vec2 TexCoords;
layout (location = 0) out vec4 FragColor;

layout (binding = 0) uniform sampler2D sceneTexture;
layout (binding = 1) uniform sampler2D bloomBlurTexture;
uniform float bloomIntensity; // Bloom intensity factor

void main() {
    vec3 sceneColor = texture(sceneTexture, TexCoords).rgb;
    vec3 bloomColor = texture(bloomBlurTexture, TexCoords).rgb * bloomIntensity;

    // Directly blend the bloom color with the scene color based on the bloom intensity
    vec3 finalColor = sceneColor + bloomColor;

    FragColor = vec4(finalColor, 1.0);
}
