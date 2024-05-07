#include "Node.h"

Node::Node(const std::string& name)
    : m_Name(name), m_Parent(nullptr), m_Position(0.0f), m_Rotation(1.0f, 0.0f, 0.0f, 0.0f), m_Scale(1.0f),
    m_Transform(1.0f), m_IsVisible(true), m_IsDirty(true) {}

Node::~Node() {}

// Hierarchy
Node* Node::getParent() const {
    return m_Parent;
}

void Node::setParent(Node* parent) {
    m_Parent = parent;
}

const std::vector<std::unique_ptr<Node>>& Node::getChildren() const {
    return m_Children;
}

void Node::addChild(std::unique_ptr<Node> child) {
    child->setParent(this);
    m_Children.push_back(std::move(child));
}

void Node::removeChild(Node* child) {
    for (auto it = m_Children.begin(); it != m_Children.end(); ++it) {
        if (it->get() == child) {
            child->setParent(nullptr);
            m_Children.erase(it);
            break;
        }
    }
}

// Transformation
const glm::vec3& Node::getPosition() const {
    return m_Position;
}

void Node::setPosition(const glm::vec3& position) {
    m_Position = position;
    m_IsDirty = true;
}

const glm::quat& Node::getRotation() const {
    return m_Rotation;
}

void Node::setRotation(const glm::quat& rotation) {
    m_Rotation = rotation;
    m_IsDirty = true;
}

const glm::vec3& Node::getScale() const {
    return m_Scale;
}

void Node::setScale(const glm::vec3& scale) {
    m_Scale = scale;
    m_IsDirty = true;
}

const glm::mat4& Node::getTransform() const {
    if (m_IsDirty) {
        m_Transform = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(m_Rotation) * glm::scale(glm::mat4(1.0f), m_Scale);
        m_IsDirty = false;
    }
    return m_Transform;
}

void Node::setTransform(const glm::mat4& transform) {
    m_Transform = transform;
    m_IsDirty = false;
}

// Animation
bool Node::hasAnimation() const {
    return m_Animation != nullptr;
}

std::shared_ptr<Animation> Node::getAnimation() const {
    return m_Animation;
}

void Node::setAnimation(std::shared_ptr<Animation> animation) {
    m_Animation = animation;
    if (m_Animation) {
        m_Animator = std::make_shared<Animator>(m_Animation);
    }
}

std::shared_ptr<Animator> Node::getAnimator() const {
    return m_Animator;
}

// Visibility
bool Node::isVisible() const {
    return m_IsVisible;
}

void Node::setVisible(bool visible) {
    m_IsVisible = visible;
}

// Update and Render
void Node::update(float deltaTime) {
    if (m_Animator) {
        m_Animator->UpdateAnimation(deltaTime);
    }

    for (const auto& child : m_Children) {
        child->update(deltaTime);
    }
}

void Node::render(const glm::mat4& parentTransform) {
    if (!m_IsVisible) {
        return;
    }

    glm::mat4 nodeTransform = parentTransform * getTransform();

    for (const auto& child : m_Children) {
        child->render(nodeTransform);
    }
}

// Misc
const std::string& Node::getName() const {
    return m_Name;
}

void Node::setName(const std::string& name) {
    m_Name = name;
}