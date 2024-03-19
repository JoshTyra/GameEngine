#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

struct Keyframe {
    float timeStamp;
    glm::vec3 position;
    glm::quat rotation; // Correct type for quaternion
    glm::vec3 scale;

    Keyframe(float timeStamp, const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale)
        : timeStamp(timeStamp), position(position), rotation(rotation), scale(scale) {}
};

