// GameStateManager.h
#pragma once
#include "GameState.h"
#include <memory> // For std::unique_ptr

class GameStateManager {
public:
    GameStateManager();
    ~GameStateManager();

    // Change to accept a pointer to a new state
    void changeState(std::unique_ptr<GameState> newState);
    void update(float deltaTime);
    void render();

private:
    std::unique_ptr<GameState> currentState;
};