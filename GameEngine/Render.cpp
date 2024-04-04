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

void Renderer::setProjectionMatrix(const glm::mat4& projectionMatrix) {
    this->projectionMatrix = projectionMatrix;
}

// Overload or new method for Renderer
void Renderer::renderFrame(const std::vector<std::shared_ptr<IRenderable>>& renderables) {
    updateFrustum(projectionMatrix * cameraController->getViewMatrix());
    frameBufferManager->bindFrameBuffer(0);
    prepareFrame();
    renderSkybox();

    // Creating a RenderingContext object here to encapsulate all the necessary state for rendering a frame.
    // This approach simplifies the method signatures of draw calls across different renderable entities and
    // makes the rendering pipeline more extensible. Instead of passing view and projection matrices separately
    // to each draw method, we now pass a single context object. This allows for easy addition of new state
    // parameters in the future without altering the interfaces of our renderable entities.
    RenderingContext context(cameraController->getViewMatrix(), projectionMatrix);

    // Draw each IRenderable using the context
    for (const auto& renderable : renderables) {
        renderable->draw(context);
    }

    frameBufferManager->unbindFrameBuffer();
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
    // Creating a RenderingContext object here to encapsulate all the necessary state for rendering a frame.
    // This approach simplifies the method signatures of draw calls across different renderable entities and
    // makes the rendering pipeline more extensible. Instead of passing view and projection matrices separately
    // to each draw method, we now pass a single context object. This allows for easy addition of new state
    // parameters in the future without altering the interfaces of our renderable entities.
    RenderingContext context(cameraController->getViewMatrix(), projectionMatrix);

    // Iterate through each geometry and render
    for (const auto& geometry : geometries) {
        if (!geometry) {
            std::cerr << "Encountered a null geometry." << std::endl;
            continue;
        }

        // Perform the frustum culling check here
        if (!geometry->isInFrustum(frustum)) {
            continue; // Skip rendering this geometry as it's outside the frustum
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

        // Now the geometry's draw method should accept a RenderingContext
        geometry->draw(context);
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
