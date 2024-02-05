#pragma once

#ifndef LEVEL_GEOMETRY_H
#define LEVEL_GEOMETRY_H

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "Texture.h"
#include "shader.h"

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
    void Draw(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection);
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

    // Method to set the shader program
    void setShader(Shader* shader) {
        this->shader = shader;
    }

    // Method to get the shader program
    Shader* getShader() const {
        return shader;
    }

private:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures; // Store textures
    GLuint VAO, VBO, EBO;
    Shader* shader;

    void setupMesh();
};

#endif // LEVEL_GEOMETRY_H