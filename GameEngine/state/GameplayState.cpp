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
}

void GameplayState::exit() {
    std::cout << "Exiting Gameplay State" << std::endl;
    // Cleanup gameplay resources, save game state if necessary
}

void GameplayState::update(float deltaTime) {
    // Ensure the camera controller is available
    auto cameraController = GameStateManager::instance().getCameraController();
    if (cameraController) {
        // Process input for the camera controller
        cameraController->processInput(deltaTime);

        // Update the camera controller's position or other properties as needed
        // This might include checking for input, moving the camera, etc.
    }
}

void GameplayState::render() {
    auto window = GameStateManager::instance().getWindowContext();
    auto cameraController = GameStateManager::instance().getCameraController();
    auto renderer = GameStateManager::instance().getRenderer();
    auto skybox = GameStateManager::instance().getSkybox(); 

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
        // Setup view and projection matrices
        auto view = cameraController->getViewMatrix();
        auto projection = glm::perspective(glm::radians(45.0f), static_cast<float>(windowWidth) / static_cast<float>(windowHeight), 0.1f, 100.0f);

        // Update renderer with the latest view and projection matrices
        renderer->setCameraController(cameraController);
        renderer->setProjectionMatrix(projection);

        // Render the skybox first
        if (skybox) {
            glDepthMask(GL_FALSE); // Turn off depth writing for skybox
            skybox->draw(view, projection);
            glDepthMask(GL_TRUE); // Turn depth writing back on
        }

        // Render each piece of geometry
        for (const auto& geometry : planeGeometry) {
            renderer->render(planeGeometry);
        }
    }
    else {
        std::cerr << "Rendering setup incomplete: Camera controller or renderer not available." << std::endl;
    }

    // Render ImGui over your scene
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


