#version 420 core

// Define subroutine type for color grading techniques
subroutine vec4 ColorGradingTechnique(vec4 color);

// Input from vertex shader
in vec2 TexCoords;

// Output color
out vec4 FragColor;

// Texture sampler for the bloom effect texture
layout(binding = 0) uniform sampler2D bloomTexture;

// Uniforms for basic color grading control
uniform float saturation;
uniform float contrast;
uniform vec3 tint;

// Subroutine implementation for basic color grading
subroutine(ColorGradingTechnique)
vec4 BasicColorGrading(vec4 color) {
    // Adjust saturation
    float lum = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722)); // Calculate luminance
    vec3 grey = vec3(lum);
    color.rgb = mix(grey, color.rgb, saturation);

    // Adjust contrast
    color.rgb = ((color.rgb - 0.5) * max(contrast, 0.0)) + 0.5;

    // Apply tint
    color.rgb += tint;

    return color;
}

// Specify which subroutine to use for color grading
subroutine uniform ColorGradingTechnique colorGrading;

void main() {
    // Fetch the color from the bloom texture
    vec4 bloomColor = texture(bloomTexture, TexCoords);

    // Apply color grading
    FragColor = colorGrading(bloomColor);
}
