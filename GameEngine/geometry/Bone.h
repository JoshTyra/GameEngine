// Bone.h
#ifndef BONE_H
#define BONE_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <vector>
#include <memory>

class Bone : public std::enable_shared_from_this<Bone> {
public:
    Bone(const std::string& name, int id, const glm::mat4& offset);

    void addChild(std::shared_ptr<Bone> child);
    void setFinalTransformation(const glm::mat4& transform);
    void calculateFinalTransformation(const glm::mat4& parentTransform);
    glm::mat4 getLocalTransform(); // Placeholder for future implementation

    glm::mat4 getFinalTransformation() const;
    std::shared_ptr<Bone> getParent() const;
    void setParent(std::shared_ptr<Bone> parent);
    glm::mat4 getOffsetMatrix() const;
    std::string getName() const;

private:
    std::string name;
    int id;
    glm::mat4 offsetMatrix;
    glm::mat4 finalTransformation;
    glm::mat4 globalInverseTransform;
    std::shared_ptr<Bone> parent;
    std::vector<std::shared_ptr<Bone>> children;
};

#endif // BONE_H
