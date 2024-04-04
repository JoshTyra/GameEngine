// IRenderable.h
#pragma once
#include <glm/glm.hpp>
#include "rendering/RenderingContext.h"

class IRenderable {
public:
    virtual ~IRenderable() = default;
    virtual void draw(const RenderingContext& context) = 0;
};

