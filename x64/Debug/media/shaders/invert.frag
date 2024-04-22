#version 420 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

void main()
{
    vec4 texColor = texture(screenTexture, TexCoords);
    FragColor = vec4(1.0 - texColor.rgb, texColor.a); // Invert the color channels, keep alpha as is
}

