// Skeleton.cpp

#include "Skeleton.h"

Skeleton::Skeleton() {
    // Initialize members if necessary
}

void Skeleton::addBone(std::shared_ptr<Bone> bone) {
    bones.push_back(bone);
    boneMapping[bone->getName()] = bone;
    // Resize the boneMatrices vector to make sure it can hold all the final transformation matrices
    boneMatrices.resize(bones.size());
}

std::shared_ptr<Bone> Skeleton::findBone(const std::string& name) {
    auto it = boneMapping.find(name);
    if (it != boneMapping.end()) {
        return it->second;
    }
    return nullptr; // Or handle the error as you see fit
}

void Skeleton::updateBoneMatricesFromAnimation(const std::map<std::string, glm::mat4>& boneTransforms) {
    // First, update each bone's finalTransformation based on the animation data
    for (const auto& boneTransform : boneTransforms) {
        const std::string& boneName = boneTransform.first;
        const glm::mat4& animTransform = boneTransform.second;

        auto boneIt = boneMapping.find(boneName);
        if (boneIt != boneMapping.end()) {
            std::shared_ptr<Bone> bone = boneIt->second;

            // Combine animation transform with bone's offset matrix
            glm::mat4 finalTransform = animTransform * bone->getOffsetMatrix();
            bone->setFinalTransformation(finalTransform);
        }
    }

    // Recalculate final transformations for the whole hierarchy
    for (auto& rootBone : getRootBones()) {
        rootBone->calculateFinalTransformation(glm::mat4(1.0f)); // Assuming the root has identity matrix as its parent transform
    }

    // Now, update the boneMatrices vector
    boneMatrices.clear();
    for (const auto& bone : bones) {
        boneMatrices.push_back(bone->getFinalTransformation());
    }
}

const std::vector<glm::mat4>& Skeleton::getBoneMatrices() const {
    return boneMatrices;
}

std::vector<std::shared_ptr<Bone>> Skeleton::getRootBones() const {
    std::vector<std::shared_ptr<Bone>> rootBones;
    for (const auto& bone : bones) {
        // A root bone is identified by not having a parent
        if (!bone->getParent()) {
            rootBones.push_back(bone);
        }
    }
    return rootBones;
}

const std::vector<std::shared_ptr<Bone>>& Skeleton::getBones() const {
    return bones;
}
