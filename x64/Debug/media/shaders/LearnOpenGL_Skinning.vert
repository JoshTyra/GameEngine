#version 430 core

layout (std140, binding = 0) uniform Uniforms {
    mat4 view;
    mat4 projection;
    vec3 cameraPositionWorld;
    vec3 cameraPositionEyeSpace;
    vec4 lightColor;
    vec3 lightDirectionWorld;
    vec3 lightDirectionEyeSpace;
    float lightIntensity;
    float nearPlane;
    float farPlane;
};

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 tex;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;
layout(location = 5) in ivec4 boneIDs;
layout(location = 6) in vec4 weights;

uniform mat4 model;

const int MAX_BONES = 40;
const int MAX_BONE_INFLUENCE = 4;

uniform mat4x3 finalBonesMatrices[MAX_BONES];

out vec2 Texcoord;
out vec3 ViewDirection;
out vec3 LightDirection;
out vec3 FragPos;

void main()
{
    mat4x3 boneMatrix = mat4x3(0.0); // Initialize to zero matrix
    for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
        if (boneIDs[i] >= 0 && boneIDs[i] < MAX_BONES) // Check valid bone ID
            boneMatrix += finalBonesMatrices[boneIDs[i]] * weights[i];
    }

    vec4 worldPosition = model * vec4(boneMatrix * vec4(pos, 1.0), 1.0);
    vec3 worldNormal = normalize(mat3(model) * (mat3(boneMatrix) * norm));
    vec3 worldTangent = normalize(mat3(model) * (mat3(boneMatrix) * tangent));
    vec3 worldBitangent = normalize(mat3(model) * (mat3(boneMatrix) * bitangent));

    vec4 viewPosition = view * worldPosition;
    gl_Position = projection * viewPosition;

    Texcoord = tex;

    vec3 viewDirectionEyeSpace = normalize(cameraPositionEyeSpace - viewPosition.xyz);
    vec3 lightDirectionEyeSpace = normalize(lightDirectionEyeSpace - viewPosition.xyz);

    mat3 TBN = transpose(mat3(worldTangent, worldBitangent, worldNormal));
    ViewDirection = TBN * viewDirectionEyeSpace;
    LightDirection = TBN * lightDirectionEyeSpace;

    FragPos = viewPosition.xyz;
}