#pragma once

#include "geometry/AnimationChannel.h"
#include "geometry/Skeleton.h"
#include <string>
#include <map>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Animation {
public:
    std::string name;
    float duration;
    float ticksPerSecond;
    std::map<std::string, AnimationChannel> channels; // Maps bone names to their animation channels

    Animation(const std::string& name, float duration, float ticksPerSecond);

    void addChannel(const AnimationChannel& channel);

    // Method to get the transformation matrix of a bone at a given animation time
    glm::mat4 getBoneTransform(float animationTime, const std::string& boneName) const;

    // Assuming you want to implement these interpolation methods
    glm::vec3 interpolatePosition(float animationTime, const AnimationChannel& channel) const;
    glm::quat interpolateRotation(float animationTime, const AnimationChannel& channel) const;
    glm::vec3 interpolateScale(float animationTime, const AnimationChannel& channel) const;

    // Update method to apply animation based on deltaTime
    void update(float deltaTime, Skeleton& skeleton); // Ensure Skeleton is defined or included
};
