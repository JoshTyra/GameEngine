// GameStateManager.cpp
#include "GameStateManager.h"
#include "MenuState.h" // Example state

GameStateManager::~GameStateManager() {
    // Perform any necessary cleanup tasks here
}

GameStateManager::GameStateManager() {
    // Initialize with a default state, for example, MenuState
    currentState = std::make_unique<MenuState>();
}

void GameStateManager::changeState(std::unique_ptr<GameState> newState) {
    if (currentState) {
        currentState->exit();
    }
    currentState = std::move(newState);
    if (currentState) {
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
        currentState->render();
    }
}