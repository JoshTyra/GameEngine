#version 420 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D screenTexture;

void main() {    
    vec3 color = texture(screenTexture, TexCoords).rgb;
    // Calculate luminance
    float luminance = dot(color, vec3(0.299, 0.587, 0.114));
    vec3 grayscale = vec3(luminance);
    
    FragColor = vec4(grayscale, 1.0);
}
