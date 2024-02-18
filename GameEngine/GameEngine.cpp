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

#include "CameraController.h"
#include "shader.h"
#include "Skybox.h"
#include "FrameTimer.h"
#include "FileSystemUtils.h"
#include "TextureLoader.h"
#include "LevelGeometry.h"
#include "ModelLoader.h"
#include "Renderer.h"
#include "AudioManager.h"
#include "Debug.h"

// Global variables
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f); // Example position
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); // Up is positive Y
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f); // Looking towards negative Z

float deltaTime = 0.0f;
float lastFrame = 0.0f;
float cameraSpeed = 6.0f;

float yaw = -90.0f;   // Yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right
float pitch = 0.0f;
float lastX = 2560.0f / 2.0f;  // Set to window width divided by 2
float lastY = 1080.0f / 2.0f;   // Set to window height divided by 2
float sensitivity = 0.1f;
bool firstMouse = true;

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
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

	// Clamping pitch value
	pitch = glm::clamp(pitch, -89.0f, 89.0f);

	glm::vec3 front{};
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}

AudioManager audioManager;

void initAudio(glm::vec3 ambiencePosition) {
	std::string audioPath = FileSystemUtils::getAssetFilePath("audio/wind2.ogg");

	// Load the sound without specifying a position
	if (!audioManager.loadSound("ambient_wind", audioPath)) {
		std::cerr << "Failed to load wind sound." << std::endl;
		return;
	}

	// Specify the position when playing the sound
	audioManager.playSound("ambient_wind", ambiencePosition, true, 1.0f, 1.0f, 500.0f);
}

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

	// Set the distance model for sound attenuation
	alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);

	// Proceed with loading sounds and other initializations
	initAudio({ 0.0f, 0.0f, 0.0f });

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330 core");

	// Enable VSync (1 = on, 0 = off)
	glfwSwapInterval(1);

	// After creating the OpenGL context, check the depth buffer size
	int depthBits;
	glGetIntegerv(GL_DEPTH_BITS, &depthBits);
	DEBUG_COUT << "Depth buffer bit depth: " << depthBits << " bits" << std::endl;

	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Enable multisampling
	glEnable(GL_MULTISAMPLE);

	// Set the key callback
	CameraController cameraController(window, cameraPos, cameraFront, cameraUp, cameraSpeed);
	glfwSetKeyCallback(window, CameraController::keyCallback);

	// Configure global OpenGL state
	glEnable(GL_DEPTH_TEST);

	// Parameters
	float nearPlane = 0.5f; // Near clipping plane
	float farPlane = 50.0f; // Far clipping plane

	// View matrix
	glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	// Model matrix
	glm::mat4 model = glm::mat4(1.0f); // Initialize to identity matrix

	// Build and compile our shader program
	Shader SkyboxShader(FileSystemUtils::getAssetFilePath("shaders/skybox.vert"), FileSystemUtils::getAssetFilePath("shaders/skybox.frag"));

	GLuint skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, skyboxVerticesSize, skyboxVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);


	std::vector<std::string> faces{
		FileSystemUtils::getAssetFilePath("skybox/clouds1_east.bmp"),   // Right
		FileSystemUtils::getAssetFilePath("skybox/clouds1_west.bmp"),   // Left
		FileSystemUtils::getAssetFilePath("skybox/clouds1_up.bmp"),     // Top
		FileSystemUtils::getAssetFilePath("skybox/clouds1_down.bmp"),   // Bottom
		FileSystemUtils::getAssetFilePath("skybox/clouds1_north.bmp"),  // Front
		FileSystemUtils::getAssetFilePath("skybox/clouds1_south.bmp")   // Back
	};

	GLuint cubemapTexture = loadCubemap(faces);

	std::vector<std::unique_ptr<LevelGeometry>> planeGeometry;

	std::string modelPath = FileSystemUtils::getAssetFilePath("models/tutorial.fbx");
	std::string materialPath = FileSystemUtils::getAssetFilePath("materials/tutorial.txt");
	planeGeometry = ModelLoader::loadModel(modelPath, materialPath);

	Renderer renderer;
	renderer.setCameraController(&cameraController);

	// Set the projection matrix once if it doesn't change often
	float aspectRatio = static_cast<float>(mode->width) / static_cast<float>(mode->height);
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
	renderer.setProjectionMatrix(projection);

	const size_t FRAME_SAMPLES = 20;  // Example value, adjust as needed
	FrameTimer frameTimer(FRAME_SAMPLES);

	while (!glfwWindowShouldClose(window)) {

		float currentFrameTime = glfwGetTime();
		float deltaTime = currentFrameTime - lastFrame;
		lastFrame = currentFrameTime;

		frameTimer.update(deltaTime);
		float smoothedDeltaTime = frameTimer.getSmoothedDeltaTime();

		cameraController.processInput(smoothedDeltaTime);

		cameraController.updateAudioListener();

		// Update positions of all active audio sources
		audioManager.updateSourcePositions();

		audioManager.cleanupSources();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImVec2 windowPos = ImVec2(mode->width - 260, 10);
		ImVec2 windowPivot = ImVec2(0.0f, 0.0f);
		ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always, windowPivot);

		// Create a window to display FPS
		ImGui::Begin("Performance");
		ImGui::Text("Average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();

		// Use the screen resolution for positioning
		ImVec2 windowPos2 = ImVec2(mode->width - 260, 70); // Adjust the X value to fit the window size
		ImVec2 windowPivot2 = ImVec2(0.0f, 0.0f); // Pivot at the top-left corner of the window
		ImGui::SetNextWindowPos(windowPos2, ImGuiCond_Always, windowPivot2);

		// Create a window to display Camera Position
		ImGui::Begin("Camera Position");
		ImGui::Text("Position: %.2f, %.2f, %.2f", cameraPos.x, cameraPos.y, cameraPos.z);
		ImGui::End();

		// Rendering
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Update view matrix
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

		// Render the skybox
		drawSkybox(skyboxVAO, cubemapTexture, SkyboxShader, view, projection);

		// Use the renderer for drawing
		renderer.render(planeGeometry);

		// Render ImGui over your scene
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Cleanup
	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &skyboxVBO);
	glDeleteTextures(1, &cubemapTexture);

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	audioManager.cleanUp();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
