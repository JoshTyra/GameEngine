#include "Animation.h"

Animation::Animation(const std::string& animationPath, const std::map<std::string, BoneInfo>& boneInfoMap)
    : m_Duration(0.0f), m_TicksPerSecond(0.0f), m_BoneInfoMap(boneInfoMap) {
    // Load the animation data from the file using Assimp
    const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
    assert(scene && scene->mRootNode);
    auto animation = scene->mAnimations[0];
    m_Duration = animation->mDuration;
    m_TicksPerSecond = animation->mTicksPerSecond;
    ReadHeirarchyData(m_RootNode, scene->mRootNode);
    ReadMissingBones(animation, boneInfoMap);
}

Bone* Animation::FindBone(const std::string& name) {
    auto iter = std::find_if(m_Bones.begin(), m_Bones.end(),
        [&](const Bone& Bone) {
            return Bone.GetBoneName() == name;
        }
    );
    if (iter == m_Bones.end()) return nullptr;
    else return &(*iter);
}

void Animation::ReadMissingBones(const aiAnimation* animation, const std::map<std::string, BoneInfo>& boneInfoMap) {
    int size = animation->mNumChannels;
    std::map<std::string, BoneInfo> localBoneInfoMap = boneInfoMap;
    int boneCount = localBoneInfoMap.size();

    for (int i = 0; i < size; i++) {
        auto channel = animation->mChannels[i];
        std::string boneName = channel->mNodeName.data;

        if (localBoneInfoMap.find(boneName) == localBoneInfoMap.end()) {
            BoneInfo& boneInfo = localBoneInfoMap[boneName];
            boneInfo.id = boneCount;
            boneCount++;
        }

        m_Bones.push_back(Bone(channel->mNodeName.data,
            localBoneInfoMap[channel->mNodeName.data].id, channel));
    }

    m_BoneInfoMap = localBoneInfoMap;
}

void Animation::ReadHeirarchyData(AssimpNodeData& dest, const aiNode* src) {
    assert(src);

    dest.name = src->mName.data;
    dest.transformation = AssimpGLMHelpers::ConvertMatrixToGLMFormat(src->mTransformation);
    dest.childrenCount = src->mNumChildren;

    if (src->mChildren && src->mNumChildren > 0) {
        for (unsigned int i = 0; i < src->mNumChildren; i++) {
            const aiNode* childNode = src->mChildren[i];
            if (childNode) {
                AssimpNodeData newData;
                ReadHeirarchyData(newData, childNode);
                dest.children.push_back(newData);
            }
            else {
                // Handle invalid child node (e.g., log an error, set default values, etc.)
                std::cerr << "Invalid child node encountered for node: " << dest.name << std::endl;
            }
        }
    }
}