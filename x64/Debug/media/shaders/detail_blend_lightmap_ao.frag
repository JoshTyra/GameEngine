#version 420 core

in vec2 TexCoords;
in vec2 LightMapTexCoords;

out vec4 FragColor;

uniform sampler2D textures[4]; // 0: Base, 1: Lightmap, 2: DetailDirt, 3: DetailGrass
uniform float TilingFactor1; // Tiling factor for the first detail texture
uniform float TilingFactor2; // Tiling factor for the second detail texture
uniform float lightmapInfluence = 0.8f; // Control this via your application
uniform float gamma = 2.2; // Gamma correction factor
uniform float brightness = 2.2f; // Brightness factor, adjust as needed

void main() {
    // Sample the base color and mask value from the base texture
    vec4 baseColor = texture(textures[0], TexCoords);
    float maskValue = baseColor.a; // Use alpha channel for the mask

    // Sample the detail textures with tiling
    vec4 dirtColor = texture(textures[2], TexCoords * TilingFactor1);
    vec4 grassColor = texture(textures[3], TexCoords * TilingFactor2);

    // Blend between the dirt and grass detail textures based on the mask value
    vec4 blendedDetail = mix(grassColor, dirtColor, maskValue);

    // Sample the lightmap texture and apply gamma correction
    vec4 lightmapColor = texture(textures[1], LightMapTexCoords);
    vec3 gammaCorrectedLightmap = pow(lightmapColor.rgb, vec3(gamma));

    // Blend the lightmap color using the influence factor
    vec3 finalLightmapColor = mix(vec3(1.0), gammaCorrectedLightmap, lightmapInfluence);

    // Combine the lightmap with the base and detail textures
    vec3 colorWithLightmap = baseColor.rgb * blendedDetail.rgb * finalLightmapColor;

    // Apply ambient occlusion
    vec3 aoColor = colorWithLightmap * lightmapColor.a;

    // Adjust final color brightness
    vec3 finalColor = aoColor * brightness;

    // Ensure the final color is within the valid range [0, 1]
    finalColor = clamp(finalColor, 0.0, 1.0);

    FragColor = vec4(finalColor, baseColor.a); // Use base alpha for the final fragment alpha
}
