#pragma once
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <memory>
#include <vector>

class RenderingContext {
public:
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
    GLuint boneMatricesBindingPoint = 0; // Default value indicating unused

    // You can add more rendering state variables here

    RenderingContext(const glm::mat4& view, const glm::mat4& projection)
        : viewMatrix(view), projectionMatrix(projection) {}

    RenderingContext(const glm::mat4& view, const glm::mat4& projection, GLuint boneBindingPoint)
        : viewMatrix(view), projectionMatrix(projection), boneMatricesBindingPoint(boneBindingPoint) {}

    // Additional methods or constructors for different contexts
};
