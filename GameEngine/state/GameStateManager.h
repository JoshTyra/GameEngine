#ifndef GAME_STATE_MANAGER_H
#define GAME_STATE_MANAGER_H

#include <memory>
#include <GLFW/glfw3.h> // Include GLFW for window context
#include "GameState.h"
#include "CameraController.h" // Include CameraController definition

class GameStateManager {
public:
    static GameStateManager& instance(); // Singleton access method

    ~GameStateManager();

    void changeState(std::unique_ptr<GameState> newState);
    void update(float deltaTime);
    void render();
    void requestExit();
    bool shouldExitRequested() const;

    // Window context and camera controller management
    void setWindowContext(GLFWwindow* window);
    GLFWwindow* getWindowContext() const;
    void setCameraController(CameraController* cameraController);
    CameraController* getCameraController() const;

private:
    GameStateManager();
    GameStateManager(const GameStateManager&) = delete; // Prevent copying
    GameStateManager& operator=(const GameStateManager&) = delete; // Prevent assignment

    std::unique_ptr<GameState> currentState;
    bool exitRequested = false;
    GLFWwindow* window = nullptr; // Holds the GLFW window context
    CameraController* cameraController = nullptr; // Holds the camera controller
};

#endif // GAME_STATE_MANAGER_H
