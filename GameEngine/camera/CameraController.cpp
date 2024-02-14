#include "CameraController.h"
#include <al.h>
#include <iostream>
#include "Debug.h"

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