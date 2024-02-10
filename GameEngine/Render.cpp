#include "Renderer.h"
#include <glm/gtc/matrix_transform.hpp>

Renderer::Renderer() : cameraController(nullptr) {}

void Renderer::setCameraController(CameraController* cameraController) {
    this->cameraController = cameraController;
}

void Renderer::setProjectionMatrix(const glm::mat4& projectionMatrix) {
    this->projectionMatrix = projectionMatrix;
}

void Renderer::render(const std::vector<std::unique_ptr<LevelGeometry>>& geometries) {
    if (!cameraController) {
        std::cerr << "Camera controller is not set." << std::endl;
        return;
    }

    glm::mat4 viewMatrix = cameraController->getViewMatrix();

    for (const auto& geometry : geometries) {
        if (!geometry) {
            std::cerr << "Encountered a null geometry." << std::endl;
            continue;
        }

        auto material = geometry->getMaterial();
        if (!material) {
            std::cerr << "Geometry missing material." << std::endl;
            continue; // Skip this geometry or handle the error as needed
        }

        auto shader = material->getShaderProgram();
        if (!shader) {
            std::cerr << "Material missing shader program." << std::endl;
            continue; // Skip this geometry or handle the error as needed
        }

        shader->use();
        shader->setMat4("view", viewMatrix);
        shader->setMat4("projection", projectionMatrix);

        glm::mat4 modelMatrix = geometry->getModelMatrix();
        shader->setMat4("model", modelMatrix);

        geometry->Draw(modelMatrix, viewMatrix, projectionMatrix);
    }
}

