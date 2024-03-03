#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "AudioManager.h"
#include "irrklang.h"

class CameraController {
public:
    CameraController(GLFWwindow* window, glm::vec3 cameraPos, glm::vec3 cameraFront, glm::vec3 cameraUp, float cameraSpeed, std::shared_ptr<AudioManager> audioManager);

    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

    void processInput(float deltaTime);
    void updateAudioListener();
    void mouseCallback(GLFWwindow* window, double xpos, double ypos);

    glm::mat4 getViewMatrix() const;

    // Accessor methods for camera properties
    glm::vec3 getCameraPosition() const { return cameraPos; }
    glm::vec3 getCameraFront() const { return cameraFront; }
    glm::vec3 getCameraUp() const { return cameraUp; }
    float getCameraSpeed() const { return cameraSpeed; }

private:
    GLFWwindow* window;
    glm::vec3 cameraPos;
    glm::vec3 cameraFront;
    glm::vec3 cameraUp;
    float cameraSpeed;
    float lastX = 2560.0f / 2.0; 
    float lastY = 1080.0f / 2.0; 
    bool firstMouse = true;
    float sensitivity = 0.1f;
    float yaw = -90.0f; 
    float pitch = 0.0f;
    std::shared_ptr<AudioManager> audioManager;

    static bool keyWPressed;
    static bool keySPressed;
    static bool keyAPressed;
    static bool keyDPressed;
};
