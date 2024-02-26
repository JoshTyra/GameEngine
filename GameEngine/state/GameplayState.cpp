#include "GameplayState.h"
#include "GameStateManager.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

void GameplayState::enter() {
    GLFWwindow* window = GameStateManager::instance().getWindowContext();
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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
    GLFWwindow* window = GameStateManager::instance().getWindowContext();
    auto cameraController = GameStateManager::instance().getCameraController(); // Use auto for simplicity

    int windowWidth, windowHeight;
    glfwGetFramebufferSize(window, &windowWidth, &windowHeight);

    // Start the new ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Example: Display FPS in an ImGui window
    ImGui::Begin("Performance");
    ImGui::Text("Average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();

    // Access the camera position
    glm::vec3 cameraPos = cameraController->getCameraPosition();

    // Create a window to display Camera Position
    ImGui::Begin("Camera Position");
    ImGui::Text("Position: %.2f, %.2f, %.2f", cameraPos.x, cameraPos.y, cameraPos.z);
    ImGui::End();

    // Clear the screen
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (cameraController) { // Check if cameraController is valid
        // Use the cameraController to get the view and projection matrices
        glm::mat4 view = cameraController->getViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), static_cast<float>(windowWidth) / static_cast<float>(windowHeight), 0.1f, 100.0f);

        // Assuming you pass these matrices to your renderer or directly to your shaders
        // Update your rendering logic here accordingly
    }
    else {
        // Handle the case where the camera controller is not available, if necessary
        std::cerr << "Camera controller not available for rendering." << std::endl;
    }

    // Render ImGui over your scene
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
