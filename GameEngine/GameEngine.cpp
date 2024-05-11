#include "GameEngine.h"
#include <iostream>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

GameEngine::GameEngine()
    : stateManager(GameStateManager::instance()), frameTimer(20),
    cameraPos(0.0f, 0.0f, 3.0f), cameraUp(0.0f, 1.0f, 0.0f), cameraFront(0.0f, 0.0f, -1.0f),
    cameraSpeed(6.0f), nearPlane(0.1f), farPlane(80.0f), // Adjusted to your suitable values
    deltaTime(0.0f), lastFrame(0.0f), window(nullptr) {
}

GameEngine::~GameEngine() {
    shutdown();
}

void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
    GLsizei length, const GLchar* message, const void* userParam) {
    // Ignore non-significant error/warning codes
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

    std::cerr << "GL Callback: " << (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "")
        << " type = " << type << ", severity = " << severity
        << ", message = " << message << "\n";
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
    auto projectionMatrix = glm::perspective(glm::radians(45.0f), aspectRatio, nearPlane, farPlane);
    renderer->setProjectionMatrix(projectionMatrix, nearPlane, farPlane);

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
    window = glfwCreateWindow(mode->width, mode->height, "OpenGL Application", monitor, NULL);
    if (!window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    // Set 'this' as the user pointer for the GLFW window
    glfwSetWindowUserPointer(window, this);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable VSync to cap frame rate to monitor's refresh rate
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Disable cursor by default

    // Set this window context in GameStateManager for global access
    GameStateManager::instance().setWindowContext(window);
}

void GameEngine::initializeOpenGL() {
    glewExperimental = GL_TRUE; // Enable full GLEW functionality
    if (glewInit() != GLEW_OK) {
        throw std::runtime_error("Failed to initialize GLEW");
    }

    // Clear any errors that might have occurred during GLEW initialization
    while (glGetError() != GL_NO_ERROR) {}

    // Check for OpenGL version support
    GLint major, minor;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    if (major < 4 || (major == 4 && minor < 3)) {
        std::cerr << "OpenGL 4.3 or higher is required. Your version: " << major << "." << minor << std::endl;
        throw std::runtime_error("Unsupported OpenGL version.");
    }

    // Setup debug message callback if supported and enabled
    GLint contextFlags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &contextFlags);
    if (contextFlags & GL_CONTEXT_FLAG_DEBUG_BIT) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(MessageCallback, nullptr);
        std::cout << "OpenGL debug context activated." << std::endl;
    }
    else {
        std::cerr << "OpenGL debug context not activated." << std::endl;
    }

    glEnable(GL_MULTISAMPLE); // Enable multisampling, assuming it's always supported

    glEnable(GL_DEPTH_TEST); // Enable depth testing
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
    ImGui_ImplOpenGL3_Init("#version 430 core");
}

void GameEngine::initializeCameraController() {

    cameraController = std::make_shared<CameraNode>(
        window, cameraPos, cameraFront, cameraUp, cameraSpeed, audioManager
    );

    GameStateManager::instance().setCameraController(cameraController);

    // Set this CameraNode instance as the user pointer for the GLFW window.
    glfwSetWindowUserPointer(window, cameraController.get());

    // Directly set static callback methods from CameraNode as GLFW callbacks.
    glfwSetKeyCallback(window, CameraNode::keyCallback);
    glfwSetCursorPosCallback(window, CameraNode::mouseCallbackStatic);
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

    auto skybox = std::make_unique<SkyboxNode>(skyboxFaces);
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
