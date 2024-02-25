#include "MenuState.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <iostream>

MenuState::MenuState(GLFWwindow* window) : window(window) {}

MenuState::~MenuState() {}

void MenuState::enter() {
    std::cout << "Entering Menu State" << std::endl;
}

void MenuState::exit() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    std::cout << "Exiting Menu State" << std::endl;
}

void MenuState::update(float deltaTime) {
    // Your update logic here
}

void MenuState::render() {
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
        // TODO: Replace this with actual state change logic
        std::cout << "Start Game button pressed." << std::endl;
        // Hide the cursor when the game starts
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    if (ImGui::Button("Exit", ImVec2(200, 0))) {
        // TODO: Implement exiting the game
        std::cout << "Exit button pressed." << std::endl;
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    ImGui::End();

    // Render the menu UI
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void MenuState::setWindowContext(GLFWwindow* newWindow) {
    window = newWindow;
}
