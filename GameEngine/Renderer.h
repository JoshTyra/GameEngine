#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include "LevelGeometry.h"
#include "CameraController.h"

class Renderer {
public:
    Renderer();
    void setCameraController(CameraController* cameraController);
    void setProjectionMatrix(const glm::mat4& projectionMatrix);
    void render(const std::vector<std::unique_ptr<LevelGeometry>>& geometries);

private:
    CameraController* cameraController;
    glm::mat4 projectionMatrix;
};
