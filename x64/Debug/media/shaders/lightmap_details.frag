#version 420 core

in vec2 TexCoords;
in vec2 LightMapTexCoords;

// Texture samplers using layout(binding) to directly state texture unit locations.
layout(binding = 0) uniform sampler2D diffuseTexture;
layout(binding = 1) uniform sampler2D emissiveTexture; // Assuming this is your lightmap
layout(binding = 2) uniform sampler2D detailTexture1;
layout(binding = 3) uniform sampler2D detailTexture2;
layout(binding = 4) uniform sampler2D maskTexture; // Mask for blending detail textures

// Gamma and brightness controls. Could be part of a uniform buffer object for easier management if you have multiple related uniforms.
uniform float gamma = 2.2; // Gamma correction factor
uniform float brightness = 2.2f; // Brightness factor, adjust as needed

out vec4 FragColor;

void main() {
    vec4 baseColor = texture(diffuseTexture, TexCoords);
    vec4 lightmapColor = texture(emissiveTexture, LightMapTexCoords);
    vec4 detailColor1 = texture(detailTexture1, TexCoords);
    vec4 detailColor2 = texture(detailTexture2, TexCoords);
    vec4 mask = texture(maskTexture, TexCoords);

    // Blend detail textures based on mask
    vec4 blendedDetail = mix(detailColor1, detailColor2, mask.r); // Assuming red channel of mask for blending factor

    // Apply lightmap and blended detail to base color
    vec3 color = baseColor.rgb * lightmapColor.rgb * blendedDetail.rgb;

    // Apply gamma correction
    color = pow(color, vec3(1.0 / gamma));

    // Adjust final color brightness
    color *= brightness;

    // Ensure the final color is within the valid range [0, 1]
    color = clamp(color, 0.0, 1.0);

    FragColor = vec4(color, baseColor.a); // Use base alpha for the final fragment alpha
}
