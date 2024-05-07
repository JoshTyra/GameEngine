#pragma once

#include "Node.h"
#include "StaticGeometry.h"
#include "geometry/AnimatedGeometry.h"

class RenderableNode : public Node {
public:
    RenderableNode(const std::string& name, std::unique_ptr<StaticGeometry> geometry);
    RenderableNode(const std::string& name, std::unique_ptr<AnimatedGeometry> geometry);
    virtual ~RenderableNode();

    virtual void render(const glm::mat4& parentTransform) override;
    void setAnimator(std::shared_ptr<Animator> animator);

private:
    std::unique_ptr<StaticGeometry> m_StaticGeometry;
    std::unique_ptr<AnimatedGeometry> m_AnimatedGeometry;
};