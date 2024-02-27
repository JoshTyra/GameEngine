#include "CameraController.h"
#include <al.h>
#include <iostream>
#include "Debug.h"

bool CameraController::keyWPressed = false;
bool CameraController::keySPressed = false;
bool CameraController::keyAPressed = false;
bool CameraController::keyDPressed = false;

// Correctly defined constructor
CameraController::CameraController(GLFWwindow* window, glm::vec3 cameraPos, glm::vec3 cameraFront, glm::vec3 cameraUp, float cameraSpeed)
    : window(window), cameraPos(cameraPos), cameraFront(cameraFront), cameraUp(cameraUp), cameraSpeed(cameraSpeed) {
}

glm::mat4 CameraController::getViewMatrix() const {
    return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

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

void CameraController::updateAudioListener() {
    // Set listener position
    alListener3f(AL_POSITION, cameraPos.x, cameraPos.y, cameraPos.z);

    // Debug output for listener position
    DEBUG_COUT << "Listener Position: X=" << cameraPos.x << " Y=" << cameraPos.y << " Z=" << cameraPos.z << std::endl;

    // Set listener orientation
    glm::vec3 cameraRight = glm::normalize(glm::cross(cameraFront, cameraUp));
    glm::vec3 cameraDirection = glm::normalize(cameraFront); // Ensure the camera direction is normalized
    ALfloat listenerOri[] = { cameraDirection.x, cameraDirection.y, cameraDirection.z, cameraUp.x, cameraUp.y, cameraUp.z };

    alListenerfv(AL_ORIENTATION, listenerOri);

    // Debug output for listener orientation
    DEBUG_COUT << "Listener Orientation (At): X=" << cameraDirection.x << " Y=" << cameraDirection.y << " Z=" << cameraDirection.z << std::endl;
    DEBUG_COUT << "Listener Orientation (Up): X=" << cameraUp.x << " Y=" << cameraUp.y << " Z=" << cameraUp.z << std::endl;
}

void CameraController::mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // Reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // Clamping the pitch value to prevent screen flip
    pitch = std::max(std::min(pitch, 89.0f), -89.0f);

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

