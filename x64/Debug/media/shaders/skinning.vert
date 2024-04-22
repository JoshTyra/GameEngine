#version 430 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in ivec4 aBoneIDs;
layout (location = 4) in vec4 aWeights;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 bones[100]; // Adjust this size to match your application's needs

out float firstBoneInfluence; // Pass the first bone's influence to the fragment shader

void main() {
    vec4 finalPosition = vec4(0.0);

    for(int i = 0; i < 4; i++)
    {
		vec4 transformedPosition = bones[aBoneIDs[i]] * vec4(aPosition, 1.0);
		finalPosition += transformedPosition * aWeights[i];
    }
    
    // For debugging: Pass the first bone's influence (weight) to the fragment shader
    firstBoneInfluence = aWeights[0]; // Adjust this as needed for your debugging purposes
    
   gl_Position = projection * view * model * finalPosition;
}
