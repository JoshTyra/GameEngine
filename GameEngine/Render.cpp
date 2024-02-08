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
    if (!cameraController) return;
    glm::mat4 viewMatrix = cameraController->getViewMatrix();
    for (const auto& geometry : geometries) {
        // Check if geometry is valid and has a material with a shader program
        if (!geometry || !geometry->getMaterial() || !geometry->getMaterial()->getShaderProgram()) {
            std::cerr << "Geometry missing material or shader program." << std::endl;
            continue; // Skip this geometry
        }
        Shader* shader = geometry->getMaterial()->getShaderProgram();
        shader->use();
        shader->setMat4("view", viewMatrix);
        shader->setMat4("projection", projectionMatrix);
        glm::mat4 modelMatrix = geometry->getModelMatrix(); // Assuming getModelMatrix() is implemented
        shader->setMat4("model", modelMatrix);
        geometry->Draw(modelMatrix, viewMatrix, projectionMatrix);
    }
}
