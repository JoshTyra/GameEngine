#include "Animation.h"

// Constructor
Animation::Animation(const std::string& name, float duration, float ticksPerSecond)
    : name(name), duration(duration), ticksPerSecond(ticksPerSecond) {}

// Add a channel to the animation
void Animation::addChannel(const AnimationChannel& channel) {
    channels.emplace(channel.boneName, channel);
}

glm::mat4 Animation::getBoneTransform(float animationTime, const std::string& boneName) const {
    auto channelIt = channels.find(boneName);
    if (channelIt == channels.end()) {
        return glm::mat4(1.0f); // Return identity matrix if bone is not animated
    }

    const AnimationChannel& channel = channelIt->second;

    // Position
    glm::vec3 position = interpolatePosition(animationTime, channel);
    // Rotation
    glm::quat rotation = interpolateRotation(animationTime, channel);
    // Scale
    glm::vec3 scale = interpolateScale(animationTime, channel);

    // Construct the transformation matrix from position, rotation, and scale
    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), position);
    glm::mat4 rotationMatrix = glm::mat4_cast(rotation); // Corrected line
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale);

    return translationMatrix * rotationMatrix * scaleMatrix;
}

glm::vec3 Animation::interpolatePosition(float animationTime, const AnimationChannel& channel) const {
    // Ensure there's at least two keyframes to interpolate between
    if (channel.keyframes.size() < 2) {
        // If there's only one keyframe, or none, return the position from the first keyframe or a default value
        return channel.keyframes.empty() ? glm::vec3() : channel.keyframes[0].position;
    }

    // Find the keyframe right before the current animation time, or the last keyframe if the time exceeds the animation
    size_t keyframeIndex = 0;
    for (size_t i = 0; i < channel.keyframes.size() - 1; i++) {
        if (animationTime < channel.keyframes[i + 1].timeStamp) {
            keyframeIndex = i;
            break;
        }
    }

    // If the animation time is beyond the last keyframe's time, clamp to the last two keyframes
    if (keyframeIndex == channel.keyframes.size() - 1) {
        keyframeIndex = channel.keyframes.size() - 2;
    }

    const Keyframe& startFrame = channel.keyframes[keyframeIndex];
    const Keyframe& endFrame = channel.keyframes[keyframeIndex + 1];

    // Safeguard against division by zero if the timestamps are the same
    float deltaTime = endFrame.timeStamp - startFrame.timeStamp;
    if (deltaTime == 0) {
        return startFrame.position;
    }

    float factor = (animationTime - startFrame.timeStamp) / deltaTime;
    return glm::mix(startFrame.position, endFrame.position, factor);
}

glm::quat Animation::interpolateRotation(float animationTime, const AnimationChannel& channel) const {
    if (channel.keyframes.size() < 2) {
        return channel.keyframes.empty() ? glm::quat() : channel.keyframes[0].rotation;
    }

    size_t keyframeIndex = 0;
    for (size_t i = 0; i < channel.keyframes.size() - 1; i++) {
        if (animationTime < channel.keyframes[i + 1].timeStamp) {
            keyframeIndex = i;
            break;
        }
    }

    if (keyframeIndex == channel.keyframes.size() - 1) {
        keyframeIndex = channel.keyframes.size() - 2;
    }

    const Keyframe& startFrame = channel.keyframes[keyframeIndex];
    const Keyframe& endFrame = channel.keyframes[keyframeIndex + 1];
    float deltaTime = endFrame.timeStamp - startFrame.timeStamp;
    if (deltaTime == 0) return startFrame.rotation;

    float factor = (animationTime - startFrame.timeStamp) / deltaTime;
    return glm::slerp(startFrame.rotation, endFrame.rotation, factor);
}

glm::vec3 Animation::interpolateScale(float animationTime, const AnimationChannel& channel) const {
    if (channel.keyframes.size() < 2) {
        return channel.keyframes.empty() ? glm::vec3(1.0f) : channel.keyframes[0].scale; // Assuming default scale is 1,1,1
    }

    size_t keyframeIndex = 0;
    for (size_t i = 0; i < channel.keyframes.size() - 1; i++) {
        if (animationTime < channel.keyframes[i + 1].timeStamp) {
            keyframeIndex = i;
            break;
        }
    }

    if (keyframeIndex == channel.keyframes.size() - 1) {
        keyframeIndex = channel.keyframes.size() - 2;
    }

    const Keyframe& startFrame = channel.keyframes[keyframeIndex];
    const Keyframe& endFrame = channel.keyframes[keyframeIndex + 1];
    float deltaTime = endFrame.timeStamp - startFrame.timeStamp;
    if (deltaTime == 0) return startFrame.scale;

    float factor = (animationTime - startFrame.timeStamp) / deltaTime;
    return glm::mix(startFrame.scale, endFrame.scale, factor);
}
