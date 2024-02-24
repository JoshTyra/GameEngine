#include "Renderer.h"
#include <glm/gtc/matrix_transform.hpp>

Renderer::~Renderer() {
    // Clean up the UBO
    glDeleteBuffers(1, &uboMatrices);
}

Renderer::Renderer() : cameraController(nullptr), projectionMatrix(glm::mat4(1.0f)) {
    glGenBuffers(1, &uboMatrices);
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboMatrices);
}

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

    // Update UBO
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(viewMatrix));
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projectionMatrix));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    for (const auto& geometry : geometries) {
        if (!geometry) {
            std::cerr << "Encountered a null geometry." << std::endl;
            continue;
        }

        auto material = geometry->getMaterial();
        if (!material) {
            std::cerr << "Geometry missing material." << std::endl;
            continue;
        }

        auto shader = material->getShaderProgram();
        if (!shader) {
            std::cerr << "Material missing shader program." << std::endl;
            continue;
        }

        shader->use();

        // Removed the direct setting of the view and projection matrices here
        // as they are now being passed via UBO.

        glm::mat4 modelMatrix = geometry->getModelMatrix();
        shader->setMat4("model", modelMatrix); // You still need to set the model matrix as it's unique per geometry

        geometry->Draw(modelMatrix, viewMatrix, projectionMatrix);
    }
}

