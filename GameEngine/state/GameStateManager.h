#pragma once

#include <memory>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Renderer.h"
#include "CameraNode.h"
#include "GameState.h"
#include "rendering/SkyboxNode.h"
#include "AudioManager.h"

class GameStateManager {
public:
    static GameStateManager& instance();

    ~GameStateManager();
    void changeState(std::unique_ptr<GameState> newState);
    void update(float deltaTime);
    void render();
    void requestExit();
    bool shouldExitRequested() const;

    void setWindowContext(GLFWwindow* window);
    GLFWwindow* getWindowContext() const;

    void setCameraController(std::shared_ptr<CameraNode> cameraController);
    std::shared_ptr<CameraNode> getCameraController() const;

    void setSkybox(std::shared_ptr<SkyboxNode> newSkybox);
    SkyboxNode* getSkybox() const;

    // Renderer access methods
    void setRenderer(std::shared_ptr<Renderer> renderer); // Assume using shared_ptr for simplicity
    std::shared_ptr<Renderer> getRenderer() const;

    void setAudioManager(std::shared_ptr<AudioManager> audioManager);
    std::shared_ptr<AudioManager> getAudioManager() const;

    void initializeCameraController(GLFWwindow* window, glm::vec3 cameraPos, glm::vec3 cameraFront, glm::vec3 cameraUp, float cameraSpeed);

private:
    GameStateManager();
    GameStateManager(const GameStateManager&) = delete;
    GameStateManager& operator=(const GameStateManager&) = delete;

    std::unique_ptr<GameState> currentState;
    bool exitRequested = false;
    GLFWwindow* window = nullptr;
    std::shared_ptr<CameraNode> cameraController;
    std::shared_ptr<SkyboxNode> skybox;
    std::shared_ptr<Renderer> renderer;
    std::shared_ptr<AudioManager> audioManager;
};
