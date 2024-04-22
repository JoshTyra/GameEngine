#version 430 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in ivec4 aBoneIndices;
layout (location = 4) in vec4 aBoneWeights;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

const int MAX_BONES = 100;
uniform mat4 boneTransforms[MAX_BONES];

void main()
{
    vec4 totalPosition = vec4(0.0);
    for (int i = 0; i < 4; i++)
    {
        if (aBoneIndices[i] == -1)
            continue;
        if (aBoneIndices[i] >= MAX_BONES)
        {
            totalPosition = vec4(aPosition, 1.0);
            break;
        }
        vec4 localPosition = boneTransforms[aBoneIndices[i]] * vec4(aPosition, 1.0);
        totalPosition += localPosition * aBoneWeights[i];
    }

    gl_Position = projection * view * model * totalPosition;
    FragPos = vec3(model * totalPosition);
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoord = aTexCoord;
}