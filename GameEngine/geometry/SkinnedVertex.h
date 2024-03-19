#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#define MAX_BONE_INFLUENCE 4

struct SkinnedVertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::ivec4 BoneIDs = glm::ivec4(0);
    glm::vec4 Weights = glm::vec4(0.0f);

    void addBoneData(unsigned int boneID, float weight) {
        for (int i = 0; i < MAX_BONE_INFLUENCE; ++i) {
            if (Weights[i] == 0.0f) {
                BoneIDs[i] = boneID;
                Weights[i] = weight;
                return;
            }
        }
        // Handle error: too many bones influencing this vertex
    }
};
