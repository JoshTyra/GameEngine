#pragma once

#include <vector>
#include <map>
#include <iostream>
#include <glm/glm.hpp>
#include "Animation.h"
#include "Bone.h"

class Animator {
public:
    Animator(std::shared_ptr<Animation> animation);
    void UpdateAnimation(float dt);
    void PlayAnimation(std::shared_ptr<Animation> pAnimation);
    void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);
    std::vector<glm::mat4> GetFinalBoneMatrices();

private:
    std::vector<glm::mat4> m_FinalBoneMatrices;
    std::shared_ptr<Animation> m_CurrentAnimation;
    float m_CurrentTime;
    float m_DeltaTime;
};