#include "CameraController.h"
#include <iostream>
#include "Debug.h"

bool CameraController::keyWPressed = false;
bool CameraController::keySPressed = false;
bool CameraController::keyAPressed = false;
bool CameraController::keyDPressed = false;

// Correctly defined constructor
CameraController::CameraController(GLFWwindow* window, glm::vec3 cameraPos, glm::vec3 cameraFront, glm::vec3 cameraUp, float cameraSpeed, std::shared_ptr<AudioManager> audioManager)
    : window(window), cameraPos(cameraPos), cameraFront(cameraFront), cameraUp(cameraUp), cameraSpeed(cameraSpeed), audioManager(audioManager)
{
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
    if (audioManager) {
        // The look direction can be derived from the camera's front vector.
        glm::vec3 lookDirection = cameraFront; // Assuming this is already normalized
        glm::vec3 upVector = cameraUp;

        // Convert glm::vec3 to irrklang::vec3df before passing.
        irrklang::vec3df position(cameraPos.x, cameraPos.y, cameraPos.z);
        irrklang::vec3df direction(lookDirection.x, lookDirection.y, lookDirection.z);
        irrklang::vec3df up(upVector.x, upVector.y, upVector.z);

        // Update the audio listener's position and orientation in irrKlang
        audioManager->updateListenerPosition(position, direction, up);
    }
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

