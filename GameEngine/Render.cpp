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
    glBufferData(GL_UNIFORM_BUFFER, 352, NULL, GL_STATIC_DRAW); // Allocate 352 bytes for the UBO
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboMatrices);
}

void Renderer::updateUniformBufferObject() {
    Uniforms uniforms;
    uniforms.viewMatrix = cameraController->getViewMatrix();
    uniforms.projectionMatrix = projectionMatrix;
    uniforms.camera.cameraPositionWorld = cameraController->getCameraPosition();
    uniforms.camera.cameraPositionEyeSpace = glm::vec3(uniforms.viewMatrix * glm::vec4(uniforms.camera.cameraPositionWorld, 1.0));
    uniforms.lighting.lightColor = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
    uniforms.lighting.lightDirectionWorld = glm::vec3(1.0f, 1.0f, 0.5f);
    uniforms.lighting.lightDirectionEyeSpace = glm::vec3(uniforms.viewMatrix * glm::vec4(uniforms.lighting.lightDirectionWorld, 0.0));
    uniforms.lighting.lightIntensity = 1.5f;
    uniforms.nearPlane = nearPlane;
    uniforms.farPlane = farPlane;

    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Uniforms), &uniforms);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Renderer::setCameraController(std::shared_ptr<CameraNode> cameraController) {
    this->cameraController = cameraController;
}

void Renderer::setProjectionMatrix(const glm::mat4& projectionMatrix, float nearPlane, float farPlane) {
    this->projectionMatrix = projectionMatrix;
    this->nearPlane = nearPlane;
    this->farPlane = farPlane;
}


void Renderer::renderFrame(Node* rootNode) {
    // Update the frustum for culling using the latest view and projection matrices
    updateFrustum(projectionMatrix * cameraController->getViewMatrix());

    // Bind the main framebuffer for rendering
    frameBufferManager->bindFrameBuffer(0);

    // Clear the frame and set initial OpenGL state for the frame
    prepareFrame();

    // Render the skybox
    renderSkybox();

    // Update all relevant UBOs with current frame data
    updateUniformBufferObject();

    // Traverse the scene graph and render each node
    rootNode->render(glm::mat4(1.0f));

    // Unbind the framebuffer and revert to the default framebuffer
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
        // Create a view matrix for the skybox that removes translation.
        glm::mat4 viewMatrixNoTranslation = glm::mat4(glm::mat3(cameraController->getViewMatrix()));

        // Explicitly set the view and projection matrices for the skybox shader.
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
