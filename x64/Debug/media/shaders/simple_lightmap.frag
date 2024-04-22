#version 430 core

in vec2 TexCoords;
in vec2 LightMapTexCoords;

out vec4 FragColor;

// Assuming you have 4 textures: base, lightmap, detail1, detail2
uniform sampler2D textures[2]; // An array of textures

void main() {
    vec4 baseColor = texture(textures[0], TexCoords); // Base texture
    vec4 lightmapColor = texture(textures[1], LightMapTexCoords); // Lightmap texture

    // Extract the ambient occlusion value from the alpha channel
    float ao = lightmapColor.a;
    
    // Correct multiplication by ensuring all operands are compatible types.
	FragColor = vec4(baseColor.rgb * lightmapColor.rgb * ao, baseColor.a);
}
