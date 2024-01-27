#pragma once
#ifndef CAMERACONTROLLER_H
#define CAMERACONTROLLER_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class CameraController {
public:
    CameraController(GLFWwindow* window, glm::vec3& cameraPos, glm::vec3& cameraFront, glm::vec3& cameraUp, float cameraSpeed);

    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

    void processInput(float deltaTime);

private:
    GLFWwindow* window;
    glm::vec3& cameraPos;
    glm::vec3& cameraFront;
    glm::vec3& cameraUp;
    float cameraSpeed;

    static bool keyWPressed;
    static bool keySPressed;
    static bool keyAPressed;
    static bool keyDPressed;
};

#endif // CAMERACONTROLLER_H