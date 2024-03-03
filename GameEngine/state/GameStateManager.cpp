#include "GameStateManager.h"
#include "CameraController.h"
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

void GameStateManager::initializeCameraController(GLFWwindow* window, glm::vec3 cameraPos, glm::vec3 cameraFront, glm::vec3 cameraUp, float cameraSpeed) {
    // Make sure audioManager is already initialized and set in GameStateManager
    if (audioManager) {
        auto cameraController = std::make_shared<CameraController>(window, cameraPos, cameraFront, cameraUp, cameraSpeed, audioManager);
        setCameraController(cameraController);
    }
    else {
        // Handle the case where audioManager is not set, if necessary
        std::cerr << "AudioManager not initialized before CameraController setup." << std::endl;
    }
}

void GameStateManager::setWindowContext(GLFWwindow* newWindow) {
    window = newWindow;
}

GLFWwindow* GameStateManager::getWindowContext() const {
    return window;
}

void GameStateManager::setCameraController(std::shared_ptr<CameraController> newCameraController) {
    cameraController = newCameraController;
}

std::shared_ptr<CameraController> GameStateManager::getCameraController() const {
    return cameraController;
}

void GameStateManager::setSkybox(std::shared_ptr<Skybox> newSkybox) {
    this->skybox = std::move(newSkybox);
    // Update the renderer with the new skybox
    if (this->renderer) {
        this->renderer->setSkybox(this->skybox);
    }
}

Skybox* GameStateManager::getSkybox() const {
    return skybox.get();
}

void GameStateManager::setRenderer(std::shared_ptr<Renderer> renderer) {
    this->renderer = renderer;
    // If a skybox has already been set, ensure the renderer knows about it.
    if (this->skybox) {
        this->renderer->setSkybox(this->skybox);
    }
}

std::shared_ptr<Renderer> GameStateManager::getRenderer() const {
    return renderer;
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
        currentState->render();
    }
    else {
        std::cout << "No current state to render." << std::endl;
    }
}

void GameStateManager::setAudioManager(std::shared_ptr<AudioManager> audioManager) {
    this->audioManager = audioManager;
}

std::shared_ptr<AudioManager> GameStateManager::getAudioManager() const {
    return audioManager;
}
