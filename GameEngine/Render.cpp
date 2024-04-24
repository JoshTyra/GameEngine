#include "Renderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

Renderer::Renderer(int width, int height, GLFWwindow* window)
    : screenWidth(width), screenHeight(height), projectionMatrix(glm::mat4(1.0f)), window(window) {
    frameBufferManager = std::make_unique<FrameBufferManager>(window);

    // Create the main scene framebuffer
    frameBufferManager->createFrameBuffer(width, height);
    frameBufferManager->createFrameBuffer(width, height);
    frameBufferManager->createFrameBuffer(width /2, height /2);
    frameBufferManager->createFrameBuffer(width /2, height /2);
    frameBufferManager->createFrameBuffer(width /2, height /2);
    frameBufferManager->createFrameBuffer(width, height);
    frameBufferManager->createFrameBuffer(width, height);

    frameBufferManager->createPostProcessingEffects(); // Setup post-processing effects
    setupUniformBufferObject(); // Continue with UBO setup
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

void Renderer::setProjectionMatrix(const glm::mat4& projectionMatrix, float nearPlane, float farPlane) {
    this->projectionMatrix = projectionMatrix;
    this->nearPlane = nearPlane;
    this->farPlane = farPlane;
}

void Renderer::renderFrame(const std::vector<std::shared_ptr<IRenderable>>& renderables) {
    updateFrustum(projectionMatrix * cameraController->getViewMatrix());
    frameBufferManager->bindFrameBuffer(0);
    prepareFrame();
    renderSkybox();

    // Get the camera position from the cameraController
    glm::vec3 cameraPos = cameraController->getCameraPosition();
    glm::vec4 lightColor = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
    float lightIntensity = 1.5f;

    // Assuming `cameraController` can provide the view matrix
    glm::mat4 viewMatrix = cameraController->getViewMatrix();

    // World space positions
    glm::vec3 cameraPositionWorld = cameraController->getCameraPosition();
    glm::vec3 lightDirectionWorld = glm::vec3(1.0f, 1.0f, 0.5f);  // Example light direction

    // Convert camera position to eye space
    glm::vec4 cameraPositionEye = viewMatrix * glm::vec4(cameraPositionWorld, 1.0);
    glm::vec3 cameraPosEyeSpace = glm::vec3(cameraPositionEye); // We use vec3 since the position is a point

    // Convert light direction to eye space (directional vector, w = 0)
    glm::vec4 lightDirectionEye = viewMatrix * glm::vec4(lightDirectionWorld, 0.0);
    glm::vec3 lightDirEyeSpace = glm::vec3(lightDirectionEye);

    RenderingContext context(cameraController->getViewMatrix(), projectionMatrix, cameraPosEyeSpace,
        lightDirEyeSpace, lightColor, lightIntensity, nearPlane, farPlane);

    // Draw each IRenderable using the context
    for (const auto& renderable : renderables) {
        renderable->draw(context);
    }

    frameBufferManager->unbindFrameBuffer();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

void Renderer::finalizeFrame() {
    // Bind back to default framebuffer for final output
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Disable depth testing since we're now just drawing a full-screen quad
    // and don't need the depth information.
    glDisable(GL_DEPTH_TEST);

    // Apply post-processing effects through FrameBufferManager
    GLuint sceneTexture = frameBufferManager->getSceneTexture();
    frameBufferManager->applyPostProcessingEffects(sceneTexture);

    // Re-enable depth testing for the next frame's regular rendering
    glEnable(GL_DEPTH_TEST);
}

void Renderer::setSkybox(std::shared_ptr<Skybox> skybox) {
    this->skybox = skybox;
}

void Renderer::updateFrustum(const glm::mat4& viewProjection) {
    frustum.update(viewProjection);
}

const glm::mat4& Renderer::getProjectionMatrix() const {
    return projectionMatrix;
}
