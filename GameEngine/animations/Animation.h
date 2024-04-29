#pragma once

#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <map>
#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "animations/Bone.h"
#include "utilities/assimp_glm_helpers.h"
#include "iostream"

struct AssimpNodeData {
    glm::mat4 transformation;
    std::string name;
    int childrenCount;
    std::vector<AssimpNodeData> children;
};

class Animation {

public:
    Animation() = default;
    Animation(const std::string& animationPath, const std::map<std::string, BoneInfo>& boneInfoMap);
    ~Animation() {}

    Bone* FindBone(const std::string& name);
    inline float GetTicksPerSecond() { return m_TicksPerSecond; }
    inline float GetDuration() { return m_Duration; }
    inline const AssimpNodeData& GetRootNode() { return m_RootNode; }
    inline const std::map<std::string, BoneInfo>& GetBoneIDMap() { return m_BoneInfoMap; }

private:
    void ReadMissingBones(const aiAnimation* animation, const std::map<std::string, BoneInfo>& boneInfoMap);
    void ReadHeirarchyData(AssimpNodeData& dest, const aiNode* src);

    float m_Duration;
    int m_TicksPerSecond;
    std::vector<Bone> m_Bones;
    AssimpNodeData m_RootNode;
    std::map<std::string, BoneInfo> m_BoneInfoMap;
    Assimp::Importer importer;
};