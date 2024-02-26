#pragma once
#include "GameState.h"

class GameplayState : public GameState {
public:
    // Assuming CameraController is managed outside, remove it from here

    void enter() override;
    void exit() override;
    void update(float deltaTime) override;
    void render() override;
};
