#include "PhysicsDebugDrawer.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

PhysicsDebugDrawer::PhysicsDebugDrawer(const std::string& vertexShaderPath, const std::string& fragmentShaderPath, Renderer* renderer, CameraController* cameraController)
    : debugShader(vertexShaderPath, fragmentShaderPath), renderer(renderer), cameraController(cameraController), m_debugMode(DBG_DrawWireframe) {
    // Initialize OpenGL resources for debug drawing (e.g., VAO, VBO)
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW); // Initial buffer, will be updated every frame

    // Position attribute
    glEnableVertexAttribArray(0); // Assuming position is location 0 in the shader
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(DebugVertex), (void*)offsetof(DebugVertex, position));

    // Color attribute
    glEnableVertexAttribArray(1); // Assuming color is location 1 in the shader
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(DebugVertex), (void*)offsetof(DebugVertex, color));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

PhysicsDebugDrawer::~PhysicsDebugDrawer() {
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}

void PhysicsDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color) {
    DebugVertex vertices[2] = {
        {{from.x(), from.y(), from.z()}, {color.x(), color.y(), color.z()}},
        {{to.x(), to.y(), to.z()}, {color.x(), color.y(), color.z()}}
    };
    lines.insert(lines.end(), vertices, vertices + 2);
}

void PhysicsDebugDrawer::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) {
    // Implementation goes here.
    // This is an example. You might want to draw a line or a point representing the contact.
    drawLine(PointOnB, PointOnB + normalOnB * distance, color);
}

void PhysicsDebugDrawer::reportErrorWarning(const char* warningString) {
    std::cerr << "Bullet Warning: " << warningString << std::endl;
}

void PhysicsDebugDrawer::draw3dText(const btVector3& location, const char* textString) {
    // Optional: Implement text rendering if needed
}

void PhysicsDebugDrawer::setDebugMode(int debugMode) {
    m_debugMode = debugMode;
}

int PhysicsDebugDrawer::getDebugMode() const {
    return m_debugMode;
}

void PhysicsDebugDrawer::render() {
    debugShader.use();

    if (!renderer || !cameraController) {
        std::cerr << "Renderer or CameraController is not set in PhysicsDebugDrawer." << std::endl;
        return;
    }

    glm::mat4 viewMatrix = cameraController->getViewMatrix();
    glm::mat4 projectionMatrix = renderer->getProjectionMatrix();

    debugShader.setMat4("view", viewMatrix);
    debugShader.setMat4("projection", projectionMatrix);

    // Bind VAO
    glBindVertexArray(VAO);

    // Update VBO with the current line data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, lines.size() * sizeof(DebugVertex), lines.data(), GL_DYNAMIC_DRAW);

    // Draw lines
    glDrawArrays(GL_LINES, 0, lines.size());

    // Unbind VAO
    glBindVertexArray(0);

    // Optionally clear the lines if you want to start fresh each frame
    lines.clear();
}

void PhysicsDebugDrawer::clearLines() {
    lines.clear();
}
