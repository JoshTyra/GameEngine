#include "GameStateManager.h"
#include "MenuState.h"
#include <iostream>

GameStateManager::GameStateManager() {
    // Initialization code here
}

GameStateManager::~GameStateManager() {
    // Cleanup code here
}

GameStateManager& GameStateManager::instance() {
    static GameStateManager instance;
    return instance;
}

// Implement other methods as necessary

void GameStateManager::setWindowContext(GLFWwindow* newWindow) {
    window = newWindow;
}

GLFWwindow* GameStateManager::getWindowContext() const {
    return window;
}

void GameStateManager::setCameraController(std::shared_ptr<CameraController> newCameraController) {
    cameraController = newCameraController;
}

std::shared_ptr<CameraController> GameStateManager::getCameraController() const {
    return cameraController;
}

void GameStateManager::setSkybox(std::unique_ptr<Skybox> newSkybox) {
    skybox = std::move(newSkybox);
}

Skybox* GameStateManager::getSkybox() const {
    return skybox.get();
}

void GameStateManager::changeState(std::unique_ptr<GameState> newState) {
    if (currentState) {
        currentState->exit();
    }
    currentState = std::move(newState);
    if (currentState && window) {
        currentState->setWindowContext(window); // Ensure the new state has the window context
        currentState->enter();
    }
}

void GameStateManager::update(float deltaTime) {
    if (currentState) {
        currentState->update(deltaTime);
    }
}

void GameStateManager::render() {
    if (currentState) {
        std::cout << "GameStateManager calling render on current state." << std::endl;
        currentState->render();
    }
    else {
        std::cout << "No current state to render." << std::endl;
    }
}
