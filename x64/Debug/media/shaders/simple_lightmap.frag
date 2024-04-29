#version 430 core

layout (std140, binding = 0) uniform Uniforms {
    mat4 view;
    mat4 projection;
    vec3 cameraPositionWorld;
    float _pad1;
    vec3 cameraPositionEyeSpace;
    float _pad2;
    vec4 lightColor;
    vec3 lightDirectionWorld;
    float _pad3;
    vec3 lightDirectionEyeSpace;
    float _pad4;
    float lightIntensity;
    float nearPlane;
    float farPlane;
    float _pad5[8]; // Increase the size of the padding array to 8 elements
};

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
