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
    glm::vec3 lightDirection;
    glm::vec3 lightColor;
    float lightIntensity;

    RenderingContext(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, const glm::vec3& cameraPosition,
        const glm::vec3& lightDirection, const glm::vec3& lightColor, float lightIntensity)
        : viewMatrix(viewMatrix), projectionMatrix(projectionMatrix), cameraPosition(cameraPosition),
        lightDirection(lightDirection), lightColor(lightColor), lightIntensity(lightIntensity) {}
};
