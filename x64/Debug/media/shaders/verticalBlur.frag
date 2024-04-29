#version 420 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D sceneTexture;
uniform float blurSize;

void main() {
    vec2 texOffset = 1.0 / textureSize(sceneTexture, 0); // Get the size of one texel
    vec3 result = texture(sceneTexture, TexCoords).rgb * 0.442776; // Central sample weight
    for(int i = 1; i <= 5; ++i) {
        result += texture(sceneTexture, TexCoords + vec2(0.0, texOffset.y * i * blurSize)).rgb * (0.442776 / 2.0) / float(i);
        result += texture(sceneTexture, TexCoords - vec2(0.0, texOffset.y * i * blurSize)).rgb * (0.442776 / 2.0) / float(i);
    }
    FragColor = vec4(result, 1.0);
}
