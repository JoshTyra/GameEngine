#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <utility>
#include "CameraController.h"
#include "LevelGeometry.h"
#include "rendering/Skybox.h"
#include "post-processing/PostProcessing.h"
#include "rendering/Frustum.h"
#include "post-processing/FrameBufferManager.h"

class Renderer {
    Frustum frustum;
public:
    Renderer(int width, int height, GLFWwindow* window);
    ~Renderer();
    void updateFrustum(const glm::mat4& viewProjection);

    void setCameraController(std::shared_ptr<CameraController> cameraController);
    void setProjectionMatrix(const glm::mat4& projectionMatrix);
    void setSkybox(std::shared_ptr<Skybox> skybox);

    // New method to encapsulate the entire frame rendering process
    void renderFrame(const std::vector<std::unique_ptr<LevelGeometry>>& geometries);
    void finalizeFrame();

private:
    void prepareFrame();
    void renderSkybox() const;
    void renderGeometries(const std::vector<std::unique_ptr<LevelGeometry>>& geometries);
    void setupUniformBufferObject();

    std::shared_ptr<CameraController> cameraController;
    glm::mat4 projectionMatrix;
    GLuint uboMatrices;
    std::shared_ptr<Skybox> skybox;
    GLuint fbo;
    GLuint fboTexture;
    std::shared_ptr<PostProcessing> postProcessing;
    int screenWidth, screenHeight;
    std::unique_ptr<FrameBufferManager> frameBufferManager;
    GLFWwindow* window;
};

