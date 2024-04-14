#ifndef GAMEPLAY_STATE_H
#define GAMEPLAY_STATE_H

#include <vector>
#include <memory>
#include "GameState.h"
#include "StaticGeometry.h"
#include "CameraController.h"
#include "Renderer.h"
#include "GameStateManager.h"
#include "FileSystemUtils.h"
#include "ModelLoader.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

class GameplayState : public GameState {
public:
    void enter() override;
    void exit() override;
    void update(float deltaTime) override;
    void render() override;

private:
    std::vector<std::shared_ptr<StaticGeometry>> staticGeometry;
    std::vector<std::shared_ptr<AnimatedGeometry>> animatedMeshes;
};

#endif // GAMEPLAY_STATE_H
