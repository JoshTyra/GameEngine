#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <imgui.h>
#include <imconfig.h>
#include <imstb_rectpack.h>
#include <imstb_textedit.h>
#include <imstb_truetype.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <btBulletDynamicsCommon.h>

#include "CameraController.h"
#include "shader.h"
#include "rendering/Skybox.h"
#include "FrameTimer.h"
#include "FileSystemUtils.h"
#include "TextureLoader.h"
#include "LevelGeometry.h"
#include "ModelLoader.h"
#include "Renderer.h"
#include "AudioManager.h"
#include "Debug.h"
#include "state/GameStateManager.h"
#include <state/GameState.h>
#include "physics/PhysicsDebugDrawer.h"
#include "state/MenuState.h"
#include "state/GameplayState.h"

// Global variables
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f); // Example position
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); // Up is positive Y
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f); // Looking towards negative Z

float deltaTime = 0.0f;
float lastFrame = 0.0f;
float cameraSpeed = 6.0f;

int main() {

	// Initialize GLFW
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW\n";
		return -1;
	}

	// Request a 32-bit depth buffer
	glfwWindowHint(GLFW_DEPTH_BITS, 32);

	// Enable 4x multisampling
	glfwWindowHint(GLFW_SAMPLES, 4);

	// Get the primary monitor
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	if (!monitor) {
		std::cerr << "Failed to get the primary monitor\n";
		glfwTerminate();
		return -1;
	}

	// Get the video mode of the monitor
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	if (!mode) {
		std::cerr << "Failed to get the video mode of the monitor\n";
		glfwTerminate();
		return -1;
	}

	// Create a fullscreen window using the screen resolution
	GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "OpenGL Skybox", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}

	// Make the window's context current
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::cerr << "Failed to initialize GLEW\n";
		return -1;
	}

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Enable VSync (1 = on, 0 = off)
	glfwSwapInterval(1);

	// After creating the OpenGL context, check the depth buffer size
	int depthBits;
	glGetIntegerv(GL_DEPTH_BITS, &depthBits);
	DEBUG_COUT << "Depth buffer bit depth: " << depthBits << " bits" << std::endl;

	// Enable multisampling
	glEnable(GL_MULTISAMPLE);

	// Initialize AudioManager first
	std::shared_ptr<AudioManager> audioManager = std::make_shared<AudioManager>();
	GameStateManager::instance().setAudioManager(audioManager);

	// Assuming you have added an `initializeCameraController` method in GameStateManager that does this:
	// Note: You need to implement this method based on the instructions provided in previous responses.
	GameStateManager::instance().initializeCameraController(
		window,
		cameraPos,    // Initial camera position
		cameraFront,  // Initial camera front vector
		cameraUp,     // Initial camera up vector
		cameraSpeed   // Camera movement speed
	);

	// Retrieve the CameraController instance from GameStateManager if needed elsewhere
	std::shared_ptr<CameraController> cameraController = GameStateManager::instance().getCameraController();

	// Callback setup remains unchanged
	glfwSetKeyCallback(window, CameraController::keyCallback);
	GLFWcursorposfun cursorPosCallback = [](GLFWwindow* window, double xpos, double ypos) {
		auto* controller = static_cast<CameraController*>(glfwGetWindowUserPointer(window));
		if (controller) {
			controller->mouseCallback(window, xpos, ypos);
		}
		};
	glfwSetCursorPosCallback(window, cursorPosCallback);
	glfwSetWindowUserPointer(window, cameraController.get());

	// Create the Renderer instance
	std::shared_ptr<Renderer> renderer = std::make_shared<Renderer>();

	// IMPORTANT: Set the CameraController in the Renderer
	renderer->setCameraController(cameraController);

	auto projectionMatrix = glm::perspective(glm::radians(45.0f), mode->width / (float)mode->height, 0.1f, 100.0f);
	renderer->setProjectionMatrix(projectionMatrix);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 420 core");

	// Retrieve the singleton instance of GameStateManager
	GameStateManager& stateManager = GameStateManager::instance();

	// Set the window context, and renderer in GameStateManager
	stateManager.setWindowContext(window);
	// The camera controller is now set through the initializeCameraController method
	stateManager.setRenderer(renderer);

	// Initialize the Skybox and set it in the GameStateManager
	std::vector<std::string> faces = {
		"clouds1_east.bmp",   // Right
		"clouds1_west.bmp",   // Left
		"clouds1_up.bmp",     // Top
		"clouds1_down.bmp",   // Bottom
		"clouds1_north.bmp",  // Front
		"clouds1_south.bmp"   // Back
	};
	auto skybox = std::make_unique<Skybox>(faces);
	stateManager.setSkybox(std::move(skybox));

	// Create and set the initial game state to MenuState
	auto menuState = std::make_unique<MenuState>();
	stateManager.changeState(std::move(menuState));

	// Configure global OpenGL state
	glEnable(GL_DEPTH_TEST);

	const size_t FRAME_SAMPLES = 20;  // Example value, adjust as needed
	FrameTimer frameTimer(FRAME_SAMPLES);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents(); // Handle window events.

		float currentFrameTime = glfwGetTime();
		deltaTime = currentFrameTime - lastFrame;
		lastFrame = currentFrameTime;
		frameTimer.update(deltaTime);
		float smoothedDeltaTime = frameTimer.getSmoothedDeltaTime();

		stateManager.update(smoothedDeltaTime);

		// Clear the screen.
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Render current game state.
		stateManager.render();

		// Swap buffers and poll IO events.
		glfwSwapBuffers(window);
	}

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
