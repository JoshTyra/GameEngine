#ifndef GAMEPLAY_STATE_H
#define GAMEPLAY_STATE_H

#include <vector>
#include <memory>
#include "GameState.h"
#include "LevelGeometry.h"
#include "CameraController.h"
#include "Renderer.h"
#include "geometry/AnimatedModel.h"

class GameplayState : public GameState {
public:
    void enter() override;
    void exit() override;
    void update(float deltaTime) override;
    void render() override;

private:
    std::vector<std::shared_ptr<IRenderable>> planeGeometry; // Model storage
    std::shared_ptr<AnimatedModel> animatedModel;
};

#endif // GAMEPLAY_STATE_H
