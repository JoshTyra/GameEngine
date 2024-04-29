#version 430 core

// Input texture coordinates from the vertex shader
// Since we're outputting a single color, we can actually comment out or remove this line
// in vec2 TexCoords;

// The texture sampler for the diffuse texture
// This can also be commented out or removed as we won't be using a texture
// uniform sampler2D diffuseTexture;

// Output variable for the fragment's color
out vec4 FragColor;

void main()
{
    // Set a specific color as the output
    // For example, a bright red color would be vec4(1.0, 0.0, 0.0, 1.0)
    // Adjust the values to get the color you desire
    FragColor = vec4(1.0, 0.0, 0.0, 1.0); // RGBA for red
}
