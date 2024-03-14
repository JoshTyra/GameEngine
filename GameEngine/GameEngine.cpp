#include "GameEngine.h"
#include <iostream>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
    GLsizei length, const GLchar* message, const void* userParam) {
    // Ignore non-significant error/warning codes
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

    std::cerr << "GL Callback: " << (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "")
        << " type = " << type << ", severity = " << severity
        << ", message = " << message << "\n";
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    // Retrieve the GameEngine instance from the window user pointer
    auto* engine = static_cast<GameEngine*>(glfwGetWindowUserPointer(window));
    if (!engine) return; // Safety check

    // Update the viewport to match the new window dimensions
    glViewport(0, 0, width, height);

    // Recalculate the aspect ratio and update the projection matrix
    float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    auto projectionMatrix = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);

    // Update the renderer's projection matrix
    engine->getRenderer()->setProjectionMatrix(projectionMatrix);
}

GameEngine::GameEngine()
    : stateManager(GameStateManager::instance()), frameTimer(20),
    cameraPos(0.0f, 0.0f, 3.0f), cameraUp(0.0f, 1.0f, 0.0f), cameraFront(0.0f, 0.0f, -1.0f),
    cameraSpeed(6.0f), deltaTime(0.0f), lastFrame(0.0f), window(nullptr) {
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
    glfwGetFramebufferSize(window, &width, &height); // Get the actual framebuffer size

    renderer = std::make_shared<Renderer>(width, height, window);
    stateManager.setRenderer(renderer);

    // Calculate the aspect ratio dynamically based on the framebuffer size
    float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    auto projectionMatrix = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
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
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}

void GameEngine::initializeGLFW() {
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
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

    // Set 'this' as the user pointer for the GLFW window
    glfwSetWindowUserPointer(window, this);
    // Set the framebuffer size callback
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
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

    GLint contextFlags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &contextFlags);
    if (contextFlags & GL_CONTEXT_FLAG_DEBUG_BIT) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(MessageCallback, nullptr);
    }
    else {
        std::cerr << "OpenGL debug context not activated." << std::endl;
    }

    glGetIntegerv(GL_DEPTH_BITS, &depthBits); // You might want to store depthBits as a member variable
    glEnable(GL_MULTISAMPLE); // Enable multisampling

    // After initializing GLEW and creating your OpenGL context
    if (GLEW_VERSION_4_3) { // Check that the OpenGL 4.3 functionality is available
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); // Makes debugging easier by allowing synchronous output
        glDebugMessageCallback(MessageCallback, 0);
    }
    else {
        std::cerr << "OpenGL 4.3 not supported" << std::endl;
    }


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
