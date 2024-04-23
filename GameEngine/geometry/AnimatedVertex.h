#pragma once

#include <glm/glm.hpp>

#define MAX_BONE_INFLUENCE 4

struct AnimatedVertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
    // bone indexes which will influence this vertex
    glm::ivec4 BoneIDs;
    // vertex weights
    glm::vec4 Weights;
};