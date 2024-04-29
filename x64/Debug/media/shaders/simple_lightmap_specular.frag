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
    float _pad5[8];
};

in vec2 TexCoords;
in vec2 LightMapTexCoords;
in vec3 WorldPos;
in vec3 WorldNormal;

out vec4 FragColor;

uniform sampler2D textures[2]; // An array of textures (diffuse, lightmap)
uniform samplerCube environmentMap;
uniform float roughness;

void main() {
    vec4 diffuseColor = texture(textures[0], TexCoords);
    float specularMask = diffuseColor.a;
    
    vec4 lightmapColor = texture(textures[1], LightMapTexCoords);
	
	// Multiply the diffuse color with the lightmap color
	vec3 lightmappedDiffuse = diffuseColor.rgb * lightmapColor.rgb;
    
    vec3 viewDirWorld = normalize(WorldPos - cameraPositionWorld);
    vec3 reflectDirWorld = reflect(-viewDirWorld, normalize(WorldNormal));
    
    vec3 reflectedColor = texture(environmentMap, reflectDirWorld).rgb;
    
    // Calculate the reflection strength based on the roughness value
    float reflectionStrength = 1.0 - roughness;
    
	// Blend the reflected color with the lightmapped diffuse color based on the specular mask and reflection strength
	vec3 finalColor = mix(lightmappedDiffuse, reflectedColor, specularMask * reflectionStrength);
    
    FragColor = vec4(finalColor, 1.0);
}