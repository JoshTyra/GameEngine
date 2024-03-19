// Skeleton.h

#ifndef SKELETON_H
#define SKELETON_H

#include "Bone.h"
#include <vector>
#include <unordered_map>

class Skeleton {
public:
    Skeleton();

    void addBone(std::shared_ptr<Bone> bone);
    std::shared_ptr<Bone> findBone(const std::string& name);
    void updateBoneMatricesFromAnimation(/* parameters defining the current animation frame */);
    const std::vector<glm::mat4>& getBoneMatrices() const;

private:
    std::vector<std::shared_ptr<Bone>> bones;
    std::unordered_map<std::string, std::shared_ptr<Bone>> boneMapping;
    std::vector<glm::mat4> boneMatrices; // Used to pass to shaders for skinning
};

#endif // SKELETON_H
