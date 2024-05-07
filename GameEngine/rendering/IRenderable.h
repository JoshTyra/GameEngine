// IRenderable.h
#pragma once
#include <glm/glm.hpp>

class IRenderable {
public:
    virtual ~IRenderable() = default;
    virtual void draw() = 0;
    virtual void update(float deltaTime) = 0;
};

