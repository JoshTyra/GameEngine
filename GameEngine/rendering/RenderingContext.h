#pragma once
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <memory>
#include <vector>

class RenderingContext {
public:
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
    glm::vec3 cameraPosition;

    RenderingContext(const glm::mat4& view, const glm::mat4& projection, const glm::vec3& camPos)
        : viewMatrix(view), projectionMatrix(projection), cameraPosition(camPos) {}

    RenderingContext(const glm::mat4& view, const glm::mat4& projection, GLuint boneBindingPoint, const glm::vec3& camPos)
        : viewMatrix(view), projectionMatrix(projection), cameraPosition(camPos) {}
};
