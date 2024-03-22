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

    animatedModel = std::make_unique<AnimatedModel>(FileSystemUtils::getAssetFilePath("shaders/skinning.vert"),
        FileSystemUtils::getAssetFilePath("shaders/skinning.frag"));
    //animatedModel->loadModel(FileSystemUtils::getAssetFilePath("models/combat_sword_idle.fbx"));
    //animatedModel->setAnimation("Take 001");

    auto audioManager = GameStateManager::instance().getAudioManager();
    if (audioManager) {
        // Directly use the fully qualified path to the sound file
        std::string audioPath = FileSystemUtils::getAssetFilePath("audio/wind2.ogg");

        // Example position using glm::vec3
        glm::vec3 ambiencePosition = glm::vec3(0.0f, 0.0f, 0.0f);

        // Convert the glm::vec3 position to irrKlang's vec3df format
        irrklang::vec3df irrPosition(ambiencePosition.x, ambiencePosition.y, ambiencePosition.z);

        // Play the sound using the direct path and converted position
        audioManager->playSound("ambient_wind", audioPath, irrPosition, true, 12.0f);
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

    if (animatedModel) {
        animatedModel->update(deltaTime);
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

    // Set the Performance window position and size
    ImVec2 perfWindowPos = ImVec2(windowWidth - 10.0f, 10.0f); // 10 pixels padding from the right & top
    ImVec2 windowPivot = ImVec2(1.0f, 0.0f); // Pivot at the top right
    ImGui::SetNextWindowPos(perfWindowPos, ImGuiCond_Always, windowPivot);
    ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f)); // Auto resize window size
    ImGui::Begin("Performance", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    ImGui::Text("Average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImVec2 perfWindowSize = ImGui::GetWindowSize(); // Get the window size for the Performance window
    ImGui::End();

    // Set the Camera Position window directly below the Performance window
    ImVec2 camPosWindowPos = ImVec2(windowWidth - 10.0f, 10.0f + perfWindowSize.y); // Offset by the Performance window's height
    ImGui::SetNextWindowPos(camPosWindowPos, ImGuiCond_Always, windowPivot);
    ImGui::SetNextWindowSize(ImVec2(perfWindowSize.x, 0.0f)); // Match the width of the Performance window
    ImGui::Begin("Camera Position", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    auto cameraPos = cameraController->getCameraPosition();
    ImGui::Text("Position: %.2f, %.2f, %.2f", cameraPos.x, cameraPos.y, cameraPos.z);
    ImGui::End();

    // Clear the screen
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!cameraController || !renderer) {
        std::cerr << "Rendering setup incomplete: Camera controller or renderer not available." << std::endl;
        return;
    }

    // Create a collection for all renderable entities.
    std::vector<std::shared_ptr<IRenderable>> renderables;

    // Correctly iterate over planeGeometry and add each geometry to renderables
    for (const auto& geometry : planeGeometry) {
        renderables.push_back(geometry);
    }

    // Add the animated model if it exists and is adapted to IRenderable.
    if (animatedModel) {
        renderables.push_back(animatedModel);
    }

    // Now let the renderer handle all renderable entities.
    renderer->renderFrame(renderables);

    // Assuming finalizeFrame method takes care of post-frame operations.
    renderer->finalizeFrame();

    // Render ImGui over your scene
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}



