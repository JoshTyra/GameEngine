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
    void setCameraController(CameraController* cameraController);
    void setProjectionMatrix(const glm::mat4& projectionMatrix);
    void render(const std::vector<std::unique_ptr<LevelGeometry>>& geometries);

    const glm::mat4& getProjectionMatrix() const {
        return projectionMatrix;
    }

private:
    CameraController* cameraController;
    glm::mat4 projectionMatrix;
    GLuint uboMatrices;
};
