#include "Animator.h"

Animator::Animator(std::shared_ptr<Animation> animation)
    : m_CurrentTime(0.0), m_CurrentAnimation(animation) 
{
    assert(m_CurrentAnimation != nullptr && "Animator received a null Animation object.");
    m_FinalBoneMatrices.reserve(40);

    for (int i = 0; i < 40; i++)
        m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
}

void Animator::UpdateAnimation(float dt) {
    //std::cout << "Updating animation with delta time: " << dt << std::endl;

    if (!m_CurrentAnimation) {
        std::cerr << "UpdateAnimation called without a valid animation.";
        return;
    }

    m_DeltaTime = dt;
    if (m_CurrentAnimation) {
        m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;
        m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
        CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
    }
}

void Animator::PlayAnimation(std::shared_ptr<Animation> pAnimation) {
    m_CurrentAnimation = pAnimation;
    m_CurrentTime = 0.0f;
}

void Animator::CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform) {
    if (!node) {
        std::cerr << "Critical Error: Node is null in CalculateBoneTransform" << std::endl;
        return; // Exit to avoid further crashes
    }

    std::string nodeName;
    if (!node->name.data()) {
        std::cerr << "Warning: Node name is null" << std::endl;
    }
    if (node->name.data() != nullptr) {
        nodeName = std::string(node->name.data(), node->name.size());
    }
    else {
        nodeName = "InvalidNodeName";
        std::cerr << "Warning: node->name is invalid, using default name" << std::endl;
    }

    glm::mat4 nodeTransform = node->transformation;

    Bone* bone = m_CurrentAnimation->FindBone(nodeName);

    if (bone) {
        bone->Update(m_CurrentTime);
        nodeTransform = bone->GetLocalTransform();
    }

    glm::mat4 globalTransformation = parentTransform * nodeTransform;

    auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
    if (boneInfoMap.find(nodeName) != boneInfoMap.end()) {
        int index = boneInfoMap[nodeName].id;
        glm::mat4 offset = boneInfoMap[nodeName].offset;
        m_FinalBoneMatrices[index] = globalTransformation * offset;
    }

    for (int i = 0; i < node->childrenCount; i++) {
        CalculateBoneTransform(&node->children[i], globalTransformation);
    }
}

std::vector<glm::mat4> Animator::GetFinalBoneMatrices() {
    return m_FinalBoneMatrices;
}