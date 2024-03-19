// IRenderable.h
#pragma once
#include <glm/glm.hpp>

class IRenderable {
public:
    virtual ~IRenderable() = default;
    virtual void draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) const = 0;
};
