#version 430 core

uniform sampler2D textures[5];

uniform vec4 fvAmbient = vec4(0.3, 0.3, 0.3, 1.0); // Low ambient light
uniform vec4 fvSpecular = vec4(0.2, 0.2, 0.2, 1.0); // Strong white specular highlights
uniform vec4 fvDiffuse = vec4(1.0, 1.0, 1.0, 1.0); // Moderate diffuse reflection
uniform float fSpecularPower = 16.0; // Specular exponent for shiny surfaces

uniform vec3 cameraPos; // Camera position in eye space, to be used for correct view direction calculation

in vec2 Texcoord;
in vec3 ViewDirection;
in vec3 LightDirection;
in vec3 FragPos; // Receive the fragment position in eye space

out vec4 FragColor;

void main(void)
{
    vec3 fvLightDirection = LightDirection;
    vec3 fvNormal = texture(textures[4], Texcoord).xyz;
	// Convert from [0, 1] to [-1, 1]
	fvNormal = fvNormal * 2.0 - 1.0;
	fvNormal.y = -fvNormal.y;
	fvNormal = normalize(fvNormal);

    // Calculate the view direction in the fragment shader using the eye space fragment position
    vec3 fvViewDirection = ViewDirection;
    float fNDotL = max(0.0, dot(fvNormal, fvLightDirection));

    vec3 fvHalfVector = normalize(fvLightDirection + fvViewDirection);
    float fNDotH = max(0.0, dot(fvNormal, fvHalfVector));
    
    vec4 fvBaseColor = texture(textures[0], Texcoord); // Fetch base texture color
    vec4 fvTotalAmbient = fvAmbient * fvBaseColor;
    vec4 fvTotalDiffuse = fvDiffuse * fNDotL * fvBaseColor;
    vec4 fvTotalSpecular = fvSpecular * pow(fNDotH, fSpecularPower);

    // Combine all components to calculate final color
     FragColor = fvTotalAmbient + fvTotalDiffuse + fvTotalSpecular;
	
    // Output only specular term for debugging
    //FragColor = vec4(fvTotalSpecular.rgb, 1.0); 
	
	//FragColor = vec4(FragPos, 1.0);
}
