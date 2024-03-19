#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "LevelGeometry.h"

class BatchRenderer {
public:
    void init();
    void begin();
    void submit(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
    void end();
    void flush();
private:
    std::vector<StaticVertex> vertices; // Use your custom Vertex structure
    std::vector<unsigned int> indices;

    GLuint VAO, VBO, EBO;
    GLsizei drawCount; // Number of indices or vertices to draw
};

