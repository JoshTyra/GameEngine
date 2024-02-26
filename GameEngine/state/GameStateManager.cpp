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

void GameStateManager::setWindowContext(GLFWwindow* window) {
    this->window = window;
}

GLFWwindow* GameStateManager::getWindowContext() const {
    return window;
}

void GameStateManager::setCameraController(CameraController* cameraController) {
    this->cameraController = cameraController;
}

CameraController* GameStateManager::getCameraController() const {
    return cameraController;
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
