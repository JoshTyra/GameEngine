#include "MenuState.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <iostream>

// Remove the constructor that takes a GLFWwindow* as we now use GameStateManager to manage the window context
MenuState::~MenuState() {}

void MenuState::enter() {
    // Setup ImGui context here if needed
}

void MenuState::exit() {
    // Clean up ImGui context here if it was set up in enter()
}

void MenuState::update(float deltaTime) {
    // Your update logic here
}

void MenuState::render() {
    // Access window context from GameStateManager
    GLFWwindow* window = GameStateManager::instance().getWindowContext();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Show the mouse cursor when in the menu
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    int windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);

    // Set the window to appear in the center of the screen
    ImGui::SetNextWindowPos(ImVec2(windowWidth / 2.0f, windowHeight / 2.0f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowFocus();

    // Begin the menu window
    ImGui::Begin("Main Menu", NULL, ImGuiWindowFlags_AlwaysAutoResize);
    if (ImGui::Button("Start Game", ImVec2(200, 0))) {
        // Use a factory or specific method to create GameplayState if needed
        auto gameplayState = std::make_unique<GameplayState>();
        GameStateManager::instance().changeState(std::move(gameplayState));

        // Hide the cursor when the game starts
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    if (ImGui::Button("Exit", ImVec2(200, 0))) {
        std::cout << "Exit button pressed." << std::endl;
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    ImGui::End();

    // Render the menu UI
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
