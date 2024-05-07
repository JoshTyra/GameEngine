#pragma once
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <memory>
#include <animations/Animation.h>
#include <animations/Animator.h>

class Node {
public:
    Node(const std::string& name = "");
    virtual ~Node();

    // Hierarchy
    Node* getParent() const;
    void setParent(Node* parent);
    const std::vector<std::unique_ptr<Node>>& getChildren() const;
    void addChild(std::unique_ptr<Node> child);
    void removeChild(Node* child);

    // Transformation
    const glm::vec3& getPosition() const;
    void setPosition(const glm::vec3& position);
    const glm::quat& getRotation() const;
    void setRotation(const glm::quat& rotation);
    const glm::vec3& getScale() const;
    void setScale(const glm::vec3& scale);
    const glm::mat4& getTransform() const;
    void setTransform(const glm::mat4& transform);

    // Animation
    bool hasAnimation() const;
    std::shared_ptr<Animation> getAnimation() const;
    void setAnimation(std::shared_ptr<Animation> animation);
    std::shared_ptr<Animator> getAnimator() const;

    // Update and Render
        // Visibility
    bool isVisible() const;
    void setVisible(bool visible);
    virtual void update(float deltaTime);
    virtual void render(const glm::mat4& parentTransform);

    // Misc
    const std::string& getName() const;
    void setName(const std::string& name);

protected:
    std::string m_Name;
    Node* m_Parent;
    std::vector<std::unique_ptr<Node>> m_Children;
    glm::vec3 m_Position;
    glm::quat m_Rotation;
    glm::vec3 m_Scale;
    mutable glm::mat4 m_Transform;
    bool m_IsVisible;
    mutable bool m_IsDirty;
    std::shared_ptr<Animation> m_Animation;
    std::shared_ptr<Animator> m_Animator;
};