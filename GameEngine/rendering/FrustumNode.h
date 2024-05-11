// FrustumNode.h
#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "Shader.h"
#include "FileSystemUtils.h"
#include "Frustum.h"
#include "CameraNode.h"

class FrustumNode {
public:
    FrustumNode();
    ~FrustumNode();

    void update(const CameraNode& cam, float aspect, float fov, float zNear, float zFar) {
        frustum = createFrustumFromCamera(cam, aspect, fov, zNear, zFar);
    }
    void draw(const glm::mat4& view, const glm::mat4& projection);

private:
    void generateFrustumVertices();
    void setupFrustum();

    std::vector<glm::vec3> vertices;
    std::vector<unsigned int> indices;

    unsigned int VAO, VBO, EBO;
    Shader shader;
    Frustum frustum;
};