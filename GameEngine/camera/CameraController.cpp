#include "CameraController.h"

bool CameraController::keyWPressed = false;
bool CameraController::keySPressed = false;
bool CameraController::keyAPressed = false;
bool CameraController::keyDPressed = false;

CameraController::CameraController(GLFWwindow* window, glm::vec3& cameraPos, glm::vec3& cameraFront, glm::vec3& cameraUp, float cameraSpeed)
    : window(window), cameraPos(cameraPos), cameraFront(cameraFront), cameraUp(cameraUp), cameraSpeed(cameraSpeed) {}

void CameraController::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_W)
        keyWPressed = (action != GLFW_RELEASE);
    if (key == GLFW_KEY_S)
        keySPressed = (action != GLFW_RELEASE);
    if (key == GLFW_KEY_A)
        keyAPressed = (action != GLFW_RELEASE);
    if (key == GLFW_KEY_D)
        keyDPressed = (action != GLFW_RELEASE);
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void CameraController::processInput(float deltaTime) {
    float speed = cameraSpeed * deltaTime;
    if (keyWPressed)
        cameraPos += speed * cameraFront;
    if (keySPressed)
        cameraPos -= speed * cameraFront;
    if (keyAPressed)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * speed;
    if (keyDPressed)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * speed;
}