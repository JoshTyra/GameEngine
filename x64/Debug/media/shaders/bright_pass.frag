#version 420 core

layout (location = 0) in vec2 TexCoords;
layout (location = 0) out vec4 FragColor;

layout (binding = 0) uniform sampler2D screenTexture;
uniform float brightnessThreshold;

void main() {
    vec3 sceneColor = texture(screenTexture, TexCoords).rgb;
    // Threshold each color channel
    vec3 brightColor = max(sceneColor - brightnessThreshold, 0.0);

    // Boost the color if any channel is above the threshold
    // This helps maintain the intensity of the color in the bloom effect
    float maxChannel = max(max(brightColor.r, brightColor.g), brightColor.b);
    if (maxChannel > 0) {
        brightColor = sceneColor * (maxChannel / (maxChannel + brightnessThreshold));
    }

    FragColor = vec4(brightColor, 1.0);
}
