// GameState.cpp
#include "GameState.h"
#include <iostream>

GameStateManager::GameStateManager() : currentState(GameState::MENU) {
    // Constructor initialization list sets the initial state
}

GameStateManager::~GameStateManager() {
    // Destructor, if you need to clean up resources
}

void GameStateManager::changeState(GameState newState) {
    currentState = newState;
}

GameState GameStateManager::getCurrentState() const {
    return currentState;
}
