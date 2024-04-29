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
in vec3 ReflectDir;

uniform float roughness;

out vec4 FragColor;

uniform sampler2D textures[2]; // An array of textures (base, lightmap)
uniform samplerCube environmentMap; // Cubemap for reflections

void main() {
    vec4 baseColor = texture(textures[0], TexCoords);
    float specularIntensity = pow(baseColor.a, 1.0);

    vec4 lightmapColor = texture(textures[1], LightMapTexCoords);

    vec3 reflectedColor = texture(environmentMap, ReflectDir).rgb;

    float ao = lightmapColor.a;

    // Calculate the Fresnel effect with Schlick's approximation
    float fresnelCoeff = 0.04 + (1.0 - 0.04) * pow(1.0 - dot(normalize(ReflectDir), vec3(0, 0, 1)), 5.0);

    // Adjust the Fresnel coefficient based on the specular intensity
    fresnelCoeff *= specularIntensity;

    // Blend reflected color with base color based on the Fresnel effect and specular intensity
    vec3 color = mix(baseColor.rgb, reflectedColor, fresnelCoeff * (1.0 - roughness));

    // Apply the lightmap and ambient occlusion
    color *= lightmapColor.rgb * ao;

    FragColor = vec4(color, 1.0);
}