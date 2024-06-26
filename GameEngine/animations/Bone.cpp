#include "Bone.h"

Bone::Bone(const std::string& name, int ID, const aiNodeAnim* channel)
    : m_Name(name), m_ID(ID), m_LocalTransform(1.0f) {
    m_NumPositions = channel->mNumPositionKeys;

    for (int positionIndex = 0; positionIndex < m_NumPositions; ++positionIndex) {
        aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
        float timeStamp = channel->mPositionKeys[positionIndex].mTime;
        KeyPosition data{ AssimpGLMHelpers::GetGLMVec(aiPosition), timeStamp };
        m_Positions.push_back(data);
    }

    m_NumRotations = channel->mNumRotationKeys;
    for (int rotationIndex = 0; rotationIndex < m_NumRotations; ++rotationIndex) {
        aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
        float timeStamp = channel->mRotationKeys[rotationIndex].mTime;
        KeyRotation data{ AssimpGLMHelpers::GetGLMQuat(aiOrientation), timeStamp };
        m_Rotations.push_back(data);
    }

    m_NumScalings = channel->mNumScalingKeys;
    for (int scalingIndex = 0; scalingIndex < m_NumScalings; ++scalingIndex) {
        aiVector3D aiScale = channel->mScalingKeys[scalingIndex].mValue;
        float timeStamp = channel->mScalingKeys[scalingIndex].mTime;
        KeyScale data{ AssimpGLMHelpers::GetGLMVec(aiScale), timeStamp };
        m_Scales.push_back(data);
    }
}

void Bone::Update(float animationTime) {
    if (m_NumPositions == 1 && m_NumRotations == 1 && m_NumScalings == 1) {
        // If there is only one keyframe for position, rotation, and scaling,
        // no need to interpolate, so we can return early
        return;
    }

    glm::mat4 translation = InterpolatePosition(animationTime);
    glm::mat4 rotation = InterpolateRotation(animationTime);
    glm::mat4 scale = InterpolateScaling(animationTime);

    glm::mat4 newLocalTransform = translation * rotation * scale;

    if (newLocalTransform != m_LocalTransform) {
        m_LocalTransform = newLocalTransform;
        m_IsDirty = true;
    }
}

int Bone::GetPositionIndex(float animationTime) {
    for (int index = 0; index < m_NumPositions - 1; ++index) {
        if (animationTime < m_Positions[index + 1].timeStamp)
            return index;
    }
    return m_NumPositions - 1; // Use the last position if time exceeds the animation time
}

int Bone::GetRotationIndex(float animationTime) {
    for (int index = 0; index < m_NumRotations - 1; ++index) {
        if (animationTime < m_Rotations[index + 1].timeStamp)
            return index;
    }
    return m_NumRotations - 1;
}

int Bone::GetScaleIndex(float animationTime) {
    for (int index = 0; index < m_NumScalings - 1; ++index) {
        if (animationTime < m_Scales[index + 1].timeStamp)
            return index;
    }
    return m_NumScalings - 1;
}

float Bone::GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime) {
    float scale = 0.0f;
    float midWayLength = animationTime - lastTimeStamp;
    float framesDiff = nextTimeStamp - lastTimeStamp;
    scale = midWayLength / framesDiff;
    return scale;
}

glm::mat4 Bone::InterpolatePosition(float animationTime) {
    if (1 == m_NumPositions)
        return glm::translate(glm::mat4(1.0f), m_Positions[0].position);

    int p0Index = GetPositionIndex(animationTime);
    int p1Index = p0Index + 1;
    float scaleFactor = GetScaleFactor(m_Positions[p0Index].timeStamp, m_Positions[p1Index].timeStamp, animationTime);
    glm::vec3 finalPosition = glm::mix(m_Positions[p0Index].position, m_Positions[p1Index].position, scaleFactor);
    return glm::translate(glm::mat4(1.0f), finalPosition);
}

glm::mat4 Bone::InterpolateRotation(float animationTime) {
    if (1 == m_NumRotations) {
        auto rotation = glm::normalize(m_Rotations[0].orientation);
        return glm::toMat4(rotation);
    }

    int p0Index = GetRotationIndex(animationTime);
    int p1Index = p0Index + 1;
    float scaleFactor = GetScaleFactor(m_Rotations[p0Index].timeStamp, m_Rotations[p1Index].timeStamp, animationTime);
    glm::quat finalRotation = glm::slerp(m_Rotations[p0Index].orientation, m_Rotations[p1Index].orientation, scaleFactor);
    finalRotation = glm::normalize(finalRotation);
    return glm::toMat4(finalRotation);
}

glm::mat4 Bone::InterpolateScaling(float animationTime) {
    if (1 == m_NumScalings)
        return glm::scale(glm::mat4(1.0f), m_Scales[0].scale);

    int p0Index = GetScaleIndex(animationTime);
    int p1Index = p0Index + 1;
    float scaleFactor = GetScaleFactor(m_Scales[p0Index].timeStamp, m_Scales[p1Index].timeStamp, animationTime);
    glm::vec3 finalScale = glm::mix(m_Scales[p0Index].scale, m_Scales[p1Index].scale, scaleFactor);
    return glm::scale(glm::mat4(1.0f), finalScale);
}
