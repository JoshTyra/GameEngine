#pragma once

#ifndef LEVEL_GEOMETRY_H
#define LEVEL_GEOMETRY_H

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "shader.h"
#include "Texture.h"

// Vertex structure
struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec2 LightMapTexCoords;
};

// LevelGeometry class
class LevelGeometry {
public:
    // Destructor
    ~LevelGeometry() {
        // Clean up OpenGL resources
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }
    LevelGeometry();
    LevelGeometry(std::vector<Vertex> vertices, std::vector<unsigned int> indices);
    void Draw(Shader& shader); // Ensure Shader class is included or declared
    void addTexture(const Texture& texture);

    // Getter function for textures
    const std::vector<Texture>& getTextures() const {
        return this->textures;
    }

    // Assuming textures is a std::vector<Texture> member of LevelGeometry
    unsigned int getDiffuseTextureID() const {
        for (const Texture& texture : textures) {
            if (texture.type == "diffuse") {
                return texture.id;
            }
        }
        return 0; // Return an invalid ID if not found
    }

    // Assuming textures is a std::vector<Texture> member of LevelGeometry
    unsigned int getLightmapTextureID() const {
        for (const Texture& texture : textures) {
            if (texture.type == "emissive") {
                return texture.id;
            }
        }
        return 0; // Return an invalid ID if not found
    }
private:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures; // Store textures
    GLuint VAO, VBO, EBO;

    void setupMesh();
};

#endif // LEVEL_GEOMETRY_H