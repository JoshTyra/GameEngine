#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include "LevelGeometry.h"
#include "CameraController.h"

class Renderer {
public:
    ~Renderer();
    Renderer();
    void setCameraController(std::shared_ptr<CameraController> cameraController); // Updated to use shared_ptr
    void setProjectionMatrix(const glm::mat4& projectionMatrix);
    void render(const std::vector<std::unique_ptr<LevelGeometry>>& geometries);

    const glm::mat4& getProjectionMatrix() const {
        return projectionMatrix;
    }

private:
    std::shared_ptr<CameraController> cameraController; // Updated to use shared_ptr
    glm::mat4 projectionMatrix;
    GLuint uboMatrices;
};
