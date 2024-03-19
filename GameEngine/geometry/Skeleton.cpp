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

void Skeleton::updateBoneMatricesFromAnimation(/* parameters defining the current animation frame */) {
    // Here you will loop through all bones, updating their transformations based on the animation data
    // Then, store the final transformation in boneMatrices
}

const std::vector<glm::mat4>& Skeleton::getBoneMatrices() const {
    return boneMatrices;
}
