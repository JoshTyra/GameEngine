#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <utility>
#include "CameraNode.h"
#include "StaticGeometry.h"
#include "rendering/SkyboxNode.h"
#include "post-processing/PostProcessing.h"
#include "rendering/Frustum.h"
#include "post-processing/FrameBufferManager.h"
#include "rendering/IRenderable.h"
#include "node/Node.h"
#include "rendering/FrustumNode.h"

struct Camera {
    glm::vec3 cameraPositionWorld;
    float _padding1;  // Ensure alignment
    glm::vec3 cameraPositionEyeSpace;
    float _padding2;  // Ensure alignment
};

struct Lighting {
    glm::vec4 lightColor;
    glm::vec3 lightDirectionWorld;
    float _padding3;  // Ensure alignment
    glm::vec3 lightDirectionEyeSpace;
    float _padding4;  // Ensure alignment
    float lightIntensity;
};

struct Uniforms {
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
    Camera camera;
    Lighting lighting;
    float nearPlane;
    float farPlane;
    float _padding5[8]; // Increase the size of the padding array to 8 elements
};

class Renderer {
    Frustum frustum;
public:
    Renderer(int width, int height, GLFWwindow* window);
    ~Renderer();

    void setCameraController(std::shared_ptr<CameraNode> cameraController);
    void setProjectionMatrix(const glm::mat4& projectionMatrix, float nearPlane, float farPlane);
    void renderFrame(Node* rootNode);
    void setSkybox(std::shared_ptr<SkyboxNode> skybox);
    void finalizeFrame();
    const glm::mat4& getProjectionMatrix() const;

private:
    void setupUniformBufferObject();
    void updateUniformBufferObject();
    void prepareFrame();
    void renderSkybox() const;
    void updateFrustum(const glm::mat4& viewProjection);

    std::shared_ptr<CameraNode> cameraController;
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
    GLuint uboMatrices;
    std::shared_ptr<SkyboxNode> skybox;
    GLuint fbo;
    GLuint fboTexture;
    std::shared_ptr<PostProcessing> postProcessing;
    int screenWidth, screenHeight;
    std::unique_ptr<FrameBufferManager> frameBufferManager;
    GLFWwindow* window;
    float nearPlane;
    float farPlane;
    FrustumNode frustumNode;
};

