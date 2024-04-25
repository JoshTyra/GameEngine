#pragma once
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <memory>
#include <vector>

class RenderingContext {
public:
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
    glm::vec3 cameraPositionWorld;
    glm::vec3 cameraPositionEye;
    glm::vec3 lightDirectionWorld;
    glm::vec3 lightDirectionEye;
    glm::vec4 lightColor;
    float lightIntensity;
    float nearPlane;
    float farPlane;

    RenderingContext(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix,
        const glm::vec3& cameraPositionWorld, const glm::vec3& cameraPositionEye,
        const glm::vec3& lightDirectionWorld, const glm::vec3& lightDirectionEye,
        const glm::vec4& lightColor, float lightIntensity,
        float nearPlane, float farPlane)
        : viewMatrix(viewMatrix), projectionMatrix(projectionMatrix),
        cameraPositionWorld(cameraPositionWorld), cameraPositionEye(cameraPositionEye),
        lightDirectionWorld(lightDirectionWorld), lightDirectionEye(lightDirectionEye),
        lightColor(lightColor), lightIntensity(lightIntensity),
        nearPlane(nearPlane), farPlane(farPlane) {}
};

