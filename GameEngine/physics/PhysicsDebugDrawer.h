#ifndef PHYSICSDEBUGDRAWER_H
#define PHYSICSDEBUGDRAWER_H

#include <LinearMath/btIDebugDraw.h>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "shader.h"
#include "FileSystemUtils.h"
#include "Renderer.h" // For accessing the Renderer
#include "CameraController.h" // For accessing the CameraController

class PhysicsDebugDrawer : public btIDebugDraw {
public:
    // Update the constructor to take both Renderer and CameraController
    PhysicsDebugDrawer(const std::string& vertexShaderPath, const std::string& fragmentShaderPath, Renderer* renderer, CameraController* cameraController);
    virtual ~PhysicsDebugDrawer();

    // Implementation of btIDebugDraw's pure virtual methods...
    virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;
    virtual void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override;
    virtual void reportErrorWarning(const char* warningString) override;
    virtual void draw3dText(const btVector3& location, const char* textString) override;
    virtual void setDebugMode(int debugMode) override;
    virtual int getDebugMode() const override;

    void render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

    void clearLines();

private:
    struct DebugVertex {
        glm::vec3 position;
        glm::vec3 color;
    };

    std::vector<DebugVertex> lines;
    int m_debugMode;
    GLuint VAO, VBO;
    Shader debugShader;
    Renderer* renderer; // Renderer instance for projection matrix
    CameraController* cameraController; // CameraController instance for view matrix

    // Additional private methods or member variables...
};

#endif // PHYSICSDEBUGDRAWER_H
