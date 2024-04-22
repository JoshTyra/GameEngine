#version 430 core

in vec2 TexCoords;
in vec2 LightMapTexCoords;
in vec3 ReflectDir;
in vec3 Normal;
in vec3 WorldPos;

uniform vec3 cameraPos;
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

	vec3 viewDir = normalize(cameraPos - WorldPos);

	// Calculate the Fresnel effect with Schlick's approximation
	float fresnelCoeff = 0.04 + (1.0 - 0.04) * pow(1.0 - dot(viewDir, Normal), 5.0);

	// Adjust the Fresnel coefficient based on the specular intensity
	fresnelCoeff *= specularIntensity;

	// Blend reflected color with base color based on the Fresnel effect and specular intensity
	vec3 color = mix(baseColor.rgb, reflectedColor, fresnelCoeff * (1.0 - roughness));

	// Apply the lightmap and ambient occlusion
	color *= lightmapColor.rgb * ao;

	FragColor = vec4(color, 1.0);
}