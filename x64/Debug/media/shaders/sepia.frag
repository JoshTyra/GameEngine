#version 420 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D screenTexture;

void main() {    
    vec3 color = texture(screenTexture, TexCoords).rgb;
    // Apply sepia tone transformation
    float gray = dot(color, vec3(0.3, 0.59, 0.11));
    vec3 sepiaColor = vec3(gray) * vec3(1.2, 1.0, 0.8);
    
    // Clamp the colors to [0, 1] range
    sepiaColor = clamp(sepiaColor, 0.0, 1.0);
    
    FragColor = vec4(sepiaColor, 1.0);
}
