#ifndef GAME_STATE_MANAGER_H
#define GAME_STATE_MANAGER_H

#include <memory>
#include <GLFW/glfw3.h> // Include GLFW here if not already included elsewhere
#include "GameState.h"

class GameStateManager {
public:
    static GameStateManager& instance(); // Singleton access method

    ~GameStateManager();
    void changeState(std::unique_ptr<GameState> newState);
    void update(float deltaTime);
    void render();
    void requestExit();
    bool shouldExitRequested() const;

    // Updated to use a more conventional name for the window pointer
    void setWindowContext(GLFWwindow* window);

private:
    GameStateManager(); // Private constructor makes this a singleton
    GameStateManager(const GameStateManager&) = delete; // Prevent copying
    GameStateManager& operator=(const GameStateManager&) = delete; // Prevent assignment

    std::unique_ptr<GameState> currentState;
    bool exitRequested = false;
    GLFWwindow* window = nullptr; // Holds the GLFW window context
};

#endif // GAME_STATE_MANAGER_H
