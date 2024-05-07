#include "RenderableNode.h"

RenderableNode::RenderableNode(const std::string& name, std::unique_ptr<StaticGeometry> geometry)
    : Node(name), m_StaticGeometry(std::move(geometry)), m_AnimatedGeometry(nullptr) {}

RenderableNode::RenderableNode(const std::string& name, std::unique_ptr<AnimatedGeometry> geometry)
    : Node(name), m_StaticGeometry(nullptr), m_AnimatedGeometry(std::move(geometry)) {}

RenderableNode::~RenderableNode() {}

void RenderableNode::render(const glm::mat4& parentTransform) {
    if (!isVisible()) {
        return;
    }

    glm::mat4 nodeTransform = parentTransform * getTransform();

    if (m_StaticGeometry) {
        m_StaticGeometry->draw(nodeTransform);
    }
    else if (m_AnimatedGeometry) {
        m_AnimatedGeometry->draw(nodeTransform, m_Animator.get());
    }

    for (const auto& child : getChildren()) {
        child->render(nodeTransform);
    }
}

void RenderableNode::setAnimator(std::shared_ptr<Animator> animator) {
    m_Animator = animator;
}