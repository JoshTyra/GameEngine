#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "Shader.h"
#include "geometry/AnimatedVertex.h"

#define MAX_BONE_INFLUENCE 4

class Mesh {
public:
    std::vector<AnimatedVertex> vertices;
    std::vector<unsigned int> indices;
    unsigned int VAO;

    Mesh(std::vector<AnimatedVertex> vertices, std::vector<unsigned int> indices);
    void Draw(Shader& shader);

private:
    unsigned int VBO, EBO;

    void setupMesh();
};