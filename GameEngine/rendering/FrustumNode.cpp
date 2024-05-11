// FrustumNode.cpp
#include "FrustumNode.h"

FrustumNode::FrustumNode()
    : VAO(0), VBO(0), EBO(0),
    shader(FileSystemUtils::getAssetFilePath("shaders/frustum.vert"),
        FileSystemUtils::getAssetFilePath("shaders/frustum.frag")) {
    generateFrustumVertices();
    setupFrustum();
}

FrustumNode::~FrustumNode() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void FrustumNode::update(const CameraNode& cam, float aspect, float fov, float zNear, float zFar) {
    // Create the frustum planes based on the camera parameters
    frustum = createFrustumFromCamera(cam, aspect, fov, zNear, zFar);
}

void FrustumNode::draw(const glm::mat4& view, const glm::mat4& projection) {
    shader.use();
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
    glLineWidth(2.0); // Set line width for visibility
    glEnable(GL_DEPTH_TEST);
    glBindVertexArray(VAO);
    glDrawElements(GL_LINES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void FrustumNode::generateFrustumVertices() {
    // Define the frustum vertices for both near and far planes
    vertices = {
        glm::vec3(-1.0f, -1.0f, -1.0f), // Near bottom left
        glm::vec3(1.0f, -1.0f, -1.0f),  // Near bottom right
        glm::vec3(1.0f, 1.0f, -1.0f),   // Near top right
        glm::vec3(-1.0f, 1.0f, -1.0f),  // Near top left
        glm::vec3(-1.0f, -1.0f, 1.0f),  // Far bottom left
        glm::vec3(1.0f, -1.0f, 1.0f),   // Far bottom right
        glm::vec3(1.0f, 1.0f, 1.0f),    // Far top right
        glm::vec3(-1.0f, 1.0f, 1.0f)    // Far top left
    };

    // Define the indices for the wireframe lines that make up the frustum
    indices = {
        0, 1, 1, 2, 2, 3, 3, 0, // Near plane
        4, 5, 5, 6, 6, 7, 7, 4, // Far plane
        0, 4, 1, 5, 2, 6, 3, 7  // Connecting lines between near and far planes
    };
}

void FrustumNode::setupFrustum() {
    // Generate and bind the vertex array object (VAO)
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Generate and bind the vertex buffer object (VBO)
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

    // Generate and bind the element buffer object (EBO)
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Set up the vertex attribute pointer
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    // Unbind the VAO, VBO, and EBO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}