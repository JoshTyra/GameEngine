#ifndef GAME_STATE_MANAGER_H
#define GAME_STATE_MANAGER_H

#include <memory>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "GameState.h"
#include "CameraController.h"
#include "rendering/Skybox.h"
#include "Renderer.h"

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

    void setCameraController(std::shared_ptr<CameraController> cameraController);
    std::shared_ptr<CameraController> getCameraController() const;

    void setSkybox(std::shared_ptr<Skybox> newSkybox);
    Skybox* getSkybox() const;

    // Renderer access methods
    void setRenderer(std::shared_ptr<Renderer> renderer); // Assume using shared_ptr for simplicity
    std::shared_ptr<Renderer> getRenderer() const;

private:
    GameStateManager();
    GameStateManager(const GameStateManager&) = delete;
    GameStateManager& operator=(const GameStateManager&) = delete;

    std::unique_ptr<GameState> currentState;
    bool exitRequested = false;
    GLFWwindow* window = nullptr;
    std::shared_ptr<CameraController> cameraController;
    std::shared_ptr<Skybox> skybox;
    std::shared_ptr<Renderer> renderer; // Renderer instance
};

#endif // GAME_STATE_MANAGER_H
