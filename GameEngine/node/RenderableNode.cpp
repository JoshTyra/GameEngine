#include "RenderableNode.h"

RenderableNode::RenderableNode(const std::string& name, std::unique_ptr<StaticGeometry> geometry)
    : Node(name), m_StaticGeometry(std::move(geometry)), m_AnimatedGeometry(nullptr) {}

RenderableNode::RenderableNode(const std::string& name, std::unique_ptr<AnimatedGeometry> geometry)
    : Node(name), m_StaticGeometry(nullptr), m_AnimatedGeometry(std::move(geometry)) {}

RenderableNode::~RenderableNode() {}

void RenderableNode::render(const glm::mat4& parentTransform, const Frustum& frustum) {
    if (!isVisible()) {
        return;
    }

    glm::mat4 nodeTransform = parentTransform * getTransform();

    if (m_StaticGeometry) {
        if (m_StaticGeometry->isInFrustum(frustum)) {
            m_StaticGeometry->draw(nodeTransform);
        }
    }
    else if (m_AnimatedGeometry) {
        m_AnimatedGeometry->draw(nodeTransform, m_Animator.get(), frustum);
    }

    for (const auto& child : getChildren()) {
        child->render(nodeTransform, frustum);
    }
}

void RenderableNode::setAnimator(std::shared_ptr<Animator> animator) {
    m_Animator = animator;
}

bool RenderableNode::isInFrustum(const Frustum& frustum) const {
    if (m_StaticGeometry) {
        return m_StaticGeometry->isInFrustum(frustum);
    }
    else if (m_AnimatedGeometry) {
        return m_AnimatedGeometry->isInFrustum(frustum);
    }
    return false;
}