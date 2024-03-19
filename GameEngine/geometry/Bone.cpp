// Bone.cpp
#include "Bone.h"

Bone::Bone(const std::string& name, int id, const glm::mat4& offset)
    : name(name), id(id), offsetMatrix(offset), finalTransformation(glm::mat4(1.0f)), parent(nullptr) {
    // You could initialize more members here if needed
}

void Bone::addChild(std::shared_ptr<Bone> child) {
    children.push_back(child);
    child->parent = shared_from_this(); // Set the parent of the child bone
}

void Bone::setFinalTransformation(const glm::mat4& transform) {
    finalTransformation = transform;
}

void Bone::calculateFinalTransformation(const glm::mat4& parentTransform) {
    glm::mat4 globalTransformation = parentTransform * getLocalTransform();
    finalTransformation = globalInverseTransform * globalTransformation * offsetMatrix;

    for (auto& child : children) {
        child->calculateFinalTransformation(globalTransformation);
    }
}

glm::mat4 Bone::getLocalTransform() {
    // This method will be replaced by the actual implementation that gets the current bone's transformation
    return glm::mat4(1.0f); // Identity matrix for now
}
