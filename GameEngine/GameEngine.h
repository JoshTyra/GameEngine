#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <imgui.h>
#include <imconfig.h>
#include <imstb_rectpack.h>
#include <imstb_textedit.h>
#include <imstb_truetype.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <btBulletDynamicsCommon.h>

#include "CameraController.h"
#include "shader.h"
#include "rendering/Skybox.h"
#include "FrameTimer.h"
#include "FileSystemUtils.h"
#include "TextureLoader.h"
#include "LevelGeometry.h"
#include "ModelLoader.h"
#include "Renderer.h"
#include "AudioManager.h"
#include "Debug.h"
#include "state/GameStateManager.h"
#include <state/GameState.h>
#include "physics/PhysicsDebugDrawer.h"
#include "state/MenuState.h"
#include "state/GameplayState.h"

class GameEngine {
public:
    GameEngine();
    ~GameEngine();

    void initialize();
    void run();
    void shutdown();

    std::shared_ptr<Renderer> getRenderer() const {
        return renderer;
    }

private:
    GLFWwindow* window;
    std::shared_ptr<Renderer> renderer;
    std::shared_ptr<AudioManager> audioManager;
    std::shared_ptr<CameraController> cameraController;
    GameStateManager& stateManager;
    FrameTimer frameTimer;
    int depthBits;

    // Camera settings
    glm::vec3 cameraPos;
    glm::vec3 cameraUp;
    glm::vec3 cameraFront;
    float cameraSpeed;

    // Frame timing
    float deltaTime;
    float lastFrame;

    void initializeGLFW();
    void initializeOpenGL();
    void initializeImGui();
    void initializeCameraController();
    void setupCallbacks();
    void initializeGameStates();
    void mainLoop();
};
