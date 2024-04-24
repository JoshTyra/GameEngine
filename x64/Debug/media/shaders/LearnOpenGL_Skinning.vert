#version 430 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 tex;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;
layout(location = 5) in ivec4 boneIDs;
layout(location = 6) in vec4 weights;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec3 lightPositionEyeSpace;
uniform vec3 cameraPos; // Added uniform for camera position

const int MAX_BONES = 40;
const int MAX_BONE_INFLUENCE = 2;

uniform mat4 finalBonesMatrices[MAX_BONES];

out vec2 Texcoord;
out vec3 ViewDirection;
out vec3 LightDirection;
out vec3 FragPos;

void main()
{
    mat4 boneMatrix = finalBonesMatrices[boneIDs[0]] * weights[0];
    boneMatrix += finalBonesMatrices[boneIDs[1]] * weights[1];
    boneMatrix += finalBonesMatrices[boneIDs[2]] * weights[2];
    boneMatrix += finalBonesMatrices[boneIDs[3]] * weights[3];

    vec4 worldPosition = model * boneMatrix * vec4(pos, 1.0);
    vec3 worldNormal = normalize(mat3(model) * mat3(boneMatrix) * norm);
    vec3 worldTangent = normalize(mat3(model) * mat3(boneMatrix) * tangent);
    vec3 worldBitangent = normalize(mat3(model) * mat3(boneMatrix) * bitangent);

    vec4 viewPosition = view * worldPosition;
    gl_Position = projection * viewPosition;

    Texcoord = tex;

    vec3 viewDirectionEyeSpace = normalize(cameraPos - viewPosition.xyz);
    vec3 lightDirectionEyeSpace = normalize(lightPositionEyeSpace - viewPosition.xyz);

    mat3 TBN = transpose(mat3(worldTangent, worldBitangent, worldNormal));
    ViewDirection = TBN * viewDirectionEyeSpace;
    LightDirection = TBN * lightDirectionEyeSpace;

    FragPos = viewPosition.xyz;
}