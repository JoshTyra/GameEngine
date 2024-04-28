#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <utility>
#include "CameraController.h"
#include "StaticGeometry.h"
#include "rendering/Skybox.h"
#include "post-processing/PostProcessing.h"
#include "rendering/Frustum.h"
#include "post-processing/FrameBufferManager.h"
#include "rendering/IRenderable.h"

struct Lighting {
    glm::vec4 lightColor;
    glm::vec3 lightDirectionWorld;
    float _padding1; // Add a padding variable to maintain 16-byte alignment
    glm::vec3 lightDirectionEyeSpace;
    float lightIntensity;
    glm::vec3 _padding2; // Add another padding variable to maintain 16-byte alignment
};

struct Camera {
    glm::vec3 cameraPositionWorld;
    float _padding1; // Add a padding variable to maintain 16-byte alignment
    glm::vec3 cameraPositionEyeSpace;
    float _padding2; // Add another padding variable to maintain 16-byte alignment
};

struct Uniforms {
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
    Camera camera;
    Lighting lighting;
    float nearPlane;
    float farPlane;
    glm::vec2 _padding; // Add a padding variable to maintain 16-byte alignment
};

class Renderer {
    Frustum frustum;
public:
    Renderer(int width, int height, GLFWwindow* window);
    ~Renderer();
    void updateFrustum(const glm::mat4& viewProjection);

    void setCameraController(std::shared_ptr<CameraController> cameraController);
    void setProjectionMatrix(const glm::mat4& projectionMatrix, float nearPlane, float farPlane);
    void setSkybox(std::shared_ptr<Skybox> skybox);
    void renderFrame(const std::vector<std::shared_ptr<IRenderable>>& renderables);
    void finalizeFrame();
    const glm::mat4& getProjectionMatrix() const;

private:
    void prepareFrame();
    void renderSkybox() const;
    void setupUniformBufferObject();
    void updateUniformBufferObject();

    std::shared_ptr<CameraController> cameraController;
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
    GLuint uboMatrices;
    std::shared_ptr<Skybox> skybox;
    GLuint fbo;
    GLuint fboTexture;
    std::shared_ptr<PostProcessing> postProcessing;
    int screenWidth, screenHeight;
    std::unique_ptr<FrameBufferManager> frameBufferManager;
    GLFWwindow* window;
    float nearPlane;
    float farPlane;
};

