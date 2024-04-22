#version 420 core

in vec2 TexCoords;
in vec2 LightMapTexCoords;

out vec4 FragColor;

uniform sampler2D textures[4]; // 0: Base, 1: Lightmap, 2: DetailTexture, 3: Unused in this shader
uniform float TilingFactor1; // Tiling factor for the detail texture
uniform float lightmapInfluence = 1.0f; // Control this via your application
uniform float gamma = 2.2; // Gamma correction factor
uniform float brightness = 1.5f; // Brightness factor, adjust as needed

void main() {
    // Sample the base color from the base texture
    vec4 baseColor = texture(textures[0], TexCoords);

    // Sample the detail texture with tiling
    vec4 detailColor = texture(textures[2], TexCoords * TilingFactor1); // Assuming detail texture is at index 2

    // Blend the base color with the detail color
    vec4 blendedColor = mix(baseColor, detailColor, baseColor.a);

    // Sample the lightmap texture and apply gamma correction
    vec4 lightmapColor = texture(textures[1], LightMapTexCoords);
    vec3 gammaCorrectedLightmap = pow(lightmapColor.rgb, vec3(gamma));

    // Blend the lightmap color using the influence factor
    vec3 finalLightmapColor = mix(vec3(1.0), gammaCorrectedLightmap, lightmapInfluence);

    // Combine the lightmap with the blended textures
    vec3 colorWithLightmap = blendedColor.rgb * finalLightmapColor;

    // Apply ambient occlusion
    vec3 aoColor = colorWithLightmap * lightmapColor.a;

    // Adjust final color brightness
    vec3 finalColor = aoColor * brightness;

    // Ensure the final color is within the valid range [0, 1]
    finalColor = clamp(finalColor, 0.0, 1.0);

    FragColor = vec4(finalColor, baseColor.a); // Use base alpha for the final fragment alpha
}
