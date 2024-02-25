// GameStateManager.cpp
#include "GameStateManager.h"
#include "MenuState.h" // Example state
#include <iostream>

void GameStateManager::setWindowContext(GLFWwindow* window) {
    this->window = window;
}

GameStateManager::~GameStateManager() {
    // Perform any necessary cleanup tasks here
}

GameStateManager& GameStateManager::instance() {
    static GameStateManager instance;
    return instance;
}

GameStateManager::GameStateManager() {
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
