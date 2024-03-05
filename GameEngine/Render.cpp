#include "Renderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

Renderer::Renderer(int width, int height)
    : screenWidth(width), screenHeight(height), projectionMatrix(glm::mat4(1.0f)) {

    // Initialize post-processing
    postProcessing = std::make_shared<PostProcessing>();

    // Set up UBO
    setupUniformBufferObject();

    // Set up FBO with provided dimensions
    setupFrameBufferObject(width, height);
}

Renderer::~Renderer() {
    glDeleteBuffers(1, &uboMatrices); // Clean up the UBO
}

void Renderer::setupUniformBufferObject() {
    glGenBuffers(1, &uboMatrices);
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboMatrices);
}

void Renderer::setCameraController(std::shared_ptr<CameraController> cameraController) {
    this->cameraController = cameraController;
}

void Renderer::setProjectionMatrix(const glm::mat4& projectionMatrix) {
    this->projectionMatrix = projectionMatrix;
}

void Renderer::renderFrame(const std::vector<std::unique_ptr<LevelGeometry>>& geometries) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Bind the FBO
    prepareFrame();
    renderSkybox();
    renderGeometries(geometries);
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Bind back to the default framebuffer
}

void Renderer::prepareFrame() {
    if (!cameraController) {
        std::cerr << "Camera controller is not set." << std::endl;
        return;
    }

    // Clear the screen
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Update the UBO for view and projection matrices
    glm::mat4 viewMatrix = cameraController->getViewMatrix();
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(viewMatrix));
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projectionMatrix));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Renderer::renderSkybox() const {
    if (skybox && cameraController) {
        glm::mat4 viewMatrixNoTranslation = glm::mat4(glm::mat3(cameraController->getViewMatrix()));
        skybox->draw(viewMatrixNoTranslation, projectionMatrix);
    }
}

void Renderer::renderGeometries(const std::vector<std::unique_ptr<LevelGeometry>>& geometries) {
    // Iterate through each geometry and render
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

        shader->use(); // Bind the shader program

        // Set shader uniforms (view and projection matrices are set globally using UBO)
        glm::mat4 modelMatrix = geometry->getModelMatrix();
        shader->setMat4("model", modelMatrix);

        // Assuming the geometry knows how to draw itself, including setting its own vertex array, textures, etc.
        geometry->Draw(modelMatrix, cameraController->getViewMatrix(), projectionMatrix);
    }
}

void Renderer::finalizeFrame() {
    postProcessing->applyActiveEffect(fboTexture);
}

void Renderer::setSkybox(std::shared_ptr<Skybox> skybox) {
    this->skybox = skybox;
}

void Renderer::setupFrameBufferObject(int width, int height) {
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glGenTextures(1, &fboTexture);
    glBindTexture(GL_TEXTURE_2D, fboTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTexture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

