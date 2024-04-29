#version 420 core

layout (location = 0) in vec2 TexCoords;
layout (location = 0) out vec4 FragColor;

layout (binding = 0) uniform sampler2D sceneTexture;
uniform float blurSize;
uniform bool isHorizontalPass; // Control the pass direction

const float weight[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main() {
    vec2 texOffset = 1.0 / textureSize(sceneTexture, 0); // Size of one texel
    vec3 result = texture(sceneTexture, TexCoords).rgb * weight[0]; // Central sample

    for (int i = 1; i <= 4; ++i) {
        vec2 offset = texOffset * float(i) * blurSize;
        if (isHorizontalPass) {
            result += texture(sceneTexture, TexCoords + vec2(offset.x, 0.0)).rgb * weight[i];
            result += texture(sceneTexture, TexCoords - vec2(offset.x, 0.0)).rgb * weight[i];
        } else {
            result += texture(sceneTexture, TexCoords + vec2(0.0, offset.y)).rgb * weight[i];
            result += texture(sceneTexture, TexCoords - vec2(0.0, offset.y)).rgb * weight[i];
        }
    }

    FragColor = vec4(result, 1.0);
}
