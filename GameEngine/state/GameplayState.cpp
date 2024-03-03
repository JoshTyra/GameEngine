#include "GameplayState.h"
#include "GameStateManager.h"
#include "FileSystemUtils.h"
#include "ModelLoader.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

void GameplayState::enter() {
    GLFWwindow* window = GameStateManager::instance().getWindowContext();
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    std::string modelPath = FileSystemUtils::getAssetFilePath("models/tutorial.fbx");
    std::string materialPath = FileSystemUtils::getAssetFilePath("materials/tutorial.txt");
    planeGeometry = ModelLoader::loadModel(modelPath, materialPath);

    auto audioManager = GameStateManager::instance().getAudioManager();
    if (audioManager) {
        // Directly use the fully qualified path to the sound file
        std::string audioPath = FileSystemUtils::getAssetFilePath("audio/wind2.ogg");

        // Example position using glm::vec3
        glm::vec3 ambiencePosition = glm::vec3(0.0f, 0.0f, 0.0f);

        // Convert the glm::vec3 position to irrKlang's vec3df format
        irrklang::vec3df irrPosition(ambiencePosition.x, ambiencePosition.y, ambiencePosition.z);

        // Play the sound using the direct path and converted position
        audioManager->playSound("ambient_wind", audioPath, irrPosition, true, 10.0f);
    }
}

void GameplayState::exit() {
    std::cout << "Exiting Gameplay State" << std::endl;
    // Cleanup gameplay resources, save game state if necessary
}

void GameplayState::update(float deltaTime) {
    // Ensure the camera controller is available
    auto cameraController = GameStateManager::instance().getCameraController();
    auto audioManager = GameStateManager::instance().getAudioManager(); // Ensure you have access to the audio manager
    if (cameraController && audioManager) {
        // Process input for the camera controller
        cameraController->processInput(deltaTime);

        // Update the audio listener position and orientation to match the camera
        glm::vec3 cameraPos = cameraController->getCameraPosition();
        glm::vec3 cameraFront = cameraController->getCameraFront();
        glm::vec3 cameraUp = cameraController->getCameraUp();

        // Convert glm::vec3 to irrklang::vec3df for irrKlang compatibility
        irrklang::vec3df irrCameraPos(cameraPos.x, cameraPos.y, cameraPos.z);
        irrklang::vec3df irrCameraFront(cameraFront.x, cameraFront.y, cameraFront.z);
        irrklang::vec3df irrCameraUp(cameraUp.x, cameraUp.y, cameraUp.z);

        // Update listener position in the audio manager
        audioManager->updateListenerPosition(irrCameraPos, irrCameraFront, irrCameraUp);
    }
}

void GameplayState::render() {
    auto window = GameStateManager::instance().getWindowContext();
    auto cameraController = GameStateManager::instance().getCameraController();
    auto renderer = GameStateManager::instance().getRenderer();

    int windowWidth, windowHeight;
    glfwGetFramebufferSize(window, &windowWidth, &windowHeight);

    // Start the new ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Display FPS in an ImGui window
    ImGui::Begin("Performance");
    ImGui::Text("Average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();

    // Create a window to display Camera Position
    auto cameraPos = cameraController->getCameraPosition();
    ImGui::Begin("Camera Position");
    ImGui::Text("Position: %.2f, %.2f, %.2f", cameraPos.x, cameraPos.y, cameraPos.z);
    ImGui::End();

    // Clear the screen
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (cameraController && renderer) {
        renderer->renderFrame(planeGeometry);
    }
    else {
        std::cerr << "Rendering setup incomplete: Camera controller or renderer not available." << std::endl;
    }

    // Render ImGui over your scene
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


