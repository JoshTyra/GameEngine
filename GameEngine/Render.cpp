#include "Renderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

Renderer::Renderer(int width, int height)
    : screenWidth(width), screenHeight(height), projectionMatrix(glm::mat4(1.0f)) {

    // Initialize post-processing
    postProcessing = std::make_shared<PostProcessing>();

    // Initialize ping-pong framebuffers for post-processing
    postProcessing->initializeFramebuffers(width, height);

    // Set up UBO
    setupUniformBufferObject();

    // Set up FBO with provided dimensions
    setupFrameBufferObject(width, height);

    // Initialize post-processing effects
    initializePostProcessing();
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
    glm::mat4 viewProjectionMatrix = projectionMatrix * cameraController->getViewMatrix();
    updateFrustum(viewProjectionMatrix);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo); // Bind the FBO
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

        // Assuming the geometry knows how to draw itself, including setting its own vertex array, textures, etc.
        geometry->Draw(modelMatrix, cameraController->getViewMatrix(), projectionMatrix);
    }
}

void Renderer::finalizeFrame() {
    // Bind back to default framebuffer for final output
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Clear the default framebuffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Disable depth testing since we're now just drawing a full-screen quad
    // and don't need the depth information.
    glDisable(GL_DEPTH_TEST);

    // Apply the post-processing effect using the texture we rendered the scene into
    postProcessing->applyEffects(fboTexture);

    // Re-enable depth testing for the next frame's regular rendering
    glEnable(GL_DEPTH_TEST);
}

void Renderer::setSkybox(std::shared_ptr<Skybox> skybox) {
    this->skybox = skybox;
}

void Renderer::setupFrameBufferObject(int width, int height) {
    // Generate and bind the Framebuffer
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // Generate the texture to hold color buffer
    glGenTextures(1, &fboTexture);
    glBindTexture(GL_TEXTURE_2D, fboTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Attach the texture to the framebuffer as its color attachment
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTexture, 0);

    // Create a renderbuffer object for depth and stencil attachment (we won't use stencil in this example)
    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    // Use a depth component format
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // Attach the renderbuffer to the framebuffer
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    // Check if the framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

    // Bind back to the default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::initializePostProcessing() {
    // Create the shader for the brightness effect
    Shader brightnessShader(FileSystemUtils::getAssetFilePath("shaders/invert.vert"), 
        FileSystemUtils::getAssetFilePath("shaders/bright_pass.frag"));

    // Create the brightness effect and configure its specific uniforms
    PostProcessingEffect brightnessEffect(brightnessShader);
    brightnessEffect.addUniform(ShaderUniform("brightnessThreshold", 0.3f)); // Set the brightness threshold value

    // Add the configured brightness effect
    postProcessing->addEffect("brightness", std::move(brightnessEffect));

    // Prepare and set the list of active effects
    std::vector<std::string> activeEffects = { "brightness" };
    postProcessing->setActiveEffects(activeEffects);
}

void Renderer::updateFrustum(const glm::mat4& viewProjection) {
    frustum.update(viewProjection);
}


