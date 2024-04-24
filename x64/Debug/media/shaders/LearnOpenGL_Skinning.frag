#version 430 core

uniform sampler2D textures[5];       // Array of textures, assuming diffuse texture is at index 0
uniform samplerCube environmentMap;  // Cubemap for reflections

uniform vec4 fvAmbient = vec4(0.3, 0.3, 0.3, 1.0); 
uniform vec4 fvSpecular = vec4(0.3, 0.3, 0.3, 1.0);
uniform vec4 lightColor;
uniform float fSpecularPower = 16.0;
uniform float reflectionStrength = 0.45; // Adjustable reflection strength
uniform float lightIntensity;

uniform vec3 cameraPos;              // Camera position in eye space

in vec2 Texcoord;
in vec3 ViewDirection;
in vec3 LightDirection;
in vec3 FragPos;

out vec4 FragColor;

float computeFresnel(vec3 viewDir, vec3 normal, float bias, float scale) {
    // Schlick's approximation for the Fresnel factor
    float fresnel = bias + (1.0 - bias) * pow(1.0 - dot(viewDir, normal), scale);
    return clamp(fresnel, 0.0, 1.0);
}

void main(void) {
    vec3 fvLightDirection = normalize(LightDirection);
    vec3 fvNormal = texture(textures[4], Texcoord).xyz; // Normal map texture
    fvNormal = fvNormal * 2.0 - 1.0; // Remap to [-1, 1]
    fvNormal.y = -fvNormal.y; // Keep the green channel flipped
    fvNormal = normalize(fvNormal);

    vec3 fvViewDirection = normalize(ViewDirection);
    float fNDotL = max(0.0, dot(fvNormal, fvLightDirection));
    vec3 fvHalfVector = normalize(fvLightDirection + fvViewDirection);
    float fNDotH = max(0.0, dot(fvNormal, fvHalfVector));
    float fresnel = computeFresnel(fvViewDirection, fvNormal, 0.04, 5.0); // Adjusted Fresnel parameters

    vec4 fvBaseColor = texture(textures[0], Texcoord); // Diffuse texture

    // Ambient component
    vec3 color = fvAmbient.rgb * fvBaseColor.rgb;

    // Diffuse component
    if (fNDotL > 0.0) {
		color += fNDotL * lightIntensity * lightColor.rgb * fvBaseColor.rgb;
	}

    // Specular component influenced by the Fresnel effect
    float spec = pow(fNDotH, fSpecularPower);
    float specularIntensity = mix(0.2, 1.0, fresnel); // Blend with a base factor for visibility
    vec3 specular = spec * specularIntensity * fvSpecular.rgb;
    color += specular;

    // Reflection component, using Fresnel
    vec3 reflectionVector = reflect(-fvViewDirection, fvNormal);
    vec4 reflectionColor = texture(environmentMap, reflectionVector);
    color += (reflectionColor.rgb * fvBaseColor.a) * reflectionStrength; // Blend with alpha for reflection

    // Final color output
    FragColor = vec4(color, fvBaseColor.a);
}

// Depth could be used for fog!
/*
#version 430 core

uniform float near;
uniform float far;

in vec2 Texcoord;
in vec3 ViewDirection;
in vec3 LightDirection;
in vec3 FragPos;

out vec4 color;

float linearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main() {
    float depth = linearizeDepth(gl_FragCoord.z);
    float depthColor = depth / (far - near); // Normalize based on the known range
    color = vec4(vec3(depthColor), 1.0);
}
*/

