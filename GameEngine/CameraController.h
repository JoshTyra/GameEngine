#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class CameraController {
public:
    CameraController(GLFWwindow* window, glm::vec3 cameraPos, glm::vec3 cameraFront, glm::vec3 cameraUp, float cameraSpeed);

    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

    void processInput(float deltaTime);
    void updateAudioListener();

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

    static bool keyWPressed;
    static bool keySPressed;
    static bool keyAPressed;
    static bool keyDPressed;
};
