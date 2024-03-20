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
    // Assume getLocalTransform() now provides the animated local transformation for this bone.
    glm::mat4 localTransformation = getLocalTransform();

    // The global transformation of this bone is its local transformation pre-multiplied by its parent's global transformation.
    glm::mat4 globalTransformation = parentTransform * localTransformation;

    // The final transformation combines the bone's global transformation with its offset matrix.
    // This final transformation matrix is what gets applied to the vertices.
    finalTransformation = globalTransformation * offsetMatrix;

    // Recursively calculate the transformation for children.
    for (auto& child : children) {
        child->calculateFinalTransformation(globalTransformation);
    }
}

glm::mat4 Bone::getLocalTransform() {
    // This method will be replaced by the actual implementation that gets the current bone's transformation
    return glm::mat4(1.0f); // Identity matrix for now
}

glm::mat4 Bone::getFinalTransformation() const {
    return finalTransformation;
}

std::shared_ptr<Bone> Bone::getParent() const {
    return parent;
}

glm::mat4 Bone::getOffsetMatrix() const {
    return offsetMatrix;
}

std::string Bone::getName() const {
    return name;
}

void Bone::setParent(std::shared_ptr<Bone> newParent) {
    parent = newParent;
}
