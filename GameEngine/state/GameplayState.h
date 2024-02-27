#ifndef GAMEPLAY_STATE_H
#define GAMEPLAY_STATE_H

#include <vector>
#include <memory>
#include "GameState.h"
#include "LevelGeometry.h" // Ensure this is correctly included or forward declared

class GameplayState : public GameState {
public:
    void enter() override;
    void exit() override;
    void update(float deltaTime) override;
    void render() override;

private:
    std::vector<std::unique_ptr<LevelGeometry>> planeGeometry; // Model storage
    // Other member variables and methods...
};

#endif // GAMEPLAY_STATE_H
