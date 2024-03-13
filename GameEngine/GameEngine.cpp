#include "GameEngine.h"
#include <iostream>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

GameEngine::GameEngine()
    : stateManager(GameStateManager::instance()), frameTimer(20),
    cameraPos(0.0f, 0.0f, 3.0f), cameraUp(0.0f, 1.0f, 0.0f), cameraFront(0.0f, 0.0f, -1.0f),
    cameraSpeed(6.0f), deltaTime(0.0f), lastFrame(0.0f) {
}

GameEngine::~GameEngine() {
    shutdown();
}

void GameEngine::initialize() {
    initializeGLFW();
    initializeOpenGL();
    audioManager = std::make_shared<AudioManager>();
    stateManager.setAudioManager(audioManager);

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    renderer = std::make_shared<Renderer>(width, height);
    stateManager.setRenderer(renderer);

    auto projectionMatrix = glm::perspective(glm::radians(45.0f), width / (float)height, 0.1f, 100.0f);
    renderer->setProjectionMatrix(projectionMatrix);
    initializeCameraController();
    renderer->setCameraController(cameraController);
    initializeImGui();
    setupCallbacks();
    initializeGameStates();
}

void GameEngine::run() {
    mainLoop();
}

void GameEngine::shutdown() {
    // Cleanup ImGui, GLFW, etc.
}

void GameEngine::initializeGLFW() {
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    glfwWindowHint(GLFW_DEPTH_BITS, 32); // Request a 32-bit depth buffer
    glfwWindowHint(GLFW_SAMPLES, 4); // Enable 4x multisampling

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    if (!monitor) {
        throw std::runtime_error("Failed to get the primary monitor");
    }

    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    if (!mode) {
        throw std::runtime_error("Failed to get the video mode of the monitor");
    }

    // Create a fullscreen window using the screen resolution
    window = glfwCreateWindow(mode->width, mode->height, "OpenGL Application", NULL, NULL);
    if (!window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable VSync to cap frame rate to monitor's refresh rate
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Disable cursor by default

    // Set this window context in GameStateManager for global access
    GameStateManager::instance().setWindowContext(window);
}

void GameEngine::initializeOpenGL() {
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        throw std::runtime_error("Failed to initialize GLEW");
    }

    glGetIntegerv(GL_DEPTH_BITS, &depthBits); // You might want to store depthBits as a member variable
    glEnable(GL_MULTISAMPLE); // Enable multisampling

    // Check the depth buffer size, if needed for debugging
    std::cout << "Depth buffer bit depth: " << depthBits << " bits\n";

    glEnable(GL_DEPTH_TEST);
}

void GameEngine::initializeImGui() {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 420 core");
}

void GameEngine::initializeCameraController() {

    cameraController = std::make_shared<CameraController>(
        window, cameraPos, cameraFront, cameraUp, cameraSpeed, audioManager
    );

    GameStateManager::instance().setCameraController(cameraController);

    // Set this CameraController instance as the user pointer for the GLFW window.
    glfwSetWindowUserPointer(window, cameraController.get());

    // Directly set static callback methods from CameraController as GLFW callbacks.
    glfwSetKeyCallback(window, CameraController::keyCallback);
    glfwSetCursorPosCallback(window, CameraController::mouseCallbackStatic);
}

void GameEngine::setupCallbacks()
{
    // Callbacks here
}

void GameEngine::initializeGameStates() {
    // Initialize and set the skybox, which might be used across different game states
    std::vector<std::string> skyboxFaces = {
        "right.tga",    // Right face of the skybox, typically positive X
        "left.tga",     // Left face of the skybox, typically negative X
        "top.tga",      // Top face of the skybox, typically positive Y
        "bottom.tga",   // Bottom face of the skybox, typically negative Y
        "front.tga",    // Front face of the skybox, typically positive Z
        "back.tga"      // Back face of the skybox, typically negative Z
    };

    auto skybox = std::make_unique<Skybox>(skyboxFaces);
    stateManager.setSkybox(std::move(skybox));

    // Initialize and transition to the initial game state, e.g., MenuState
    auto menuState = std::make_unique<MenuState>();
    stateManager.changeState(std::move(menuState));
}

void GameEngine::mainLoop() {
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        float currentFrameTime = glfwGetTime();
        deltaTime = currentFrameTime - lastFrame;
        lastFrame = currentFrameTime;

        stateManager.update(deltaTime);

        stateManager.render();
        glfwSwapBuffers(window);
    }
}
