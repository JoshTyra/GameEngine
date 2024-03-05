#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include "CameraController.h"
#include "LevelGeometry.h"
#include "rendering/Skybox.h"
#include "post-processing/PostProcessingShader.h"

class Renderer {
public:
    Renderer(int width, int height);
    ~Renderer();

    void setCameraController(std::shared_ptr<CameraController> cameraController);
    void setProjectionMatrix(const glm::mat4& projectionMatrix);
    void setSkybox(std::shared_ptr<Skybox> skybox);

    // New method to encapsulate the entire frame rendering process
    void renderFrame(const std::vector<std::unique_ptr<LevelGeometry>>& geometries);

private:
    void prepareFrame();
    void renderSkybox() const;
    void renderGeometries(const std::vector<std::unique_ptr<LevelGeometry>>& geometries);
    void finalizeFrame();
    void setupUniformBufferObject();
    void setupFrameBufferObject(int width, int height);

    std::shared_ptr<CameraController> cameraController;
    glm::mat4 projectionMatrix;
    GLuint uboMatrices;
    std::shared_ptr<Skybox> skybox;
    GLuint fbo;
    GLuint fboTexture;
    std::shared_ptr<PostProcessing> postProcessing;
    int screenWidth, screenHeight;
};

