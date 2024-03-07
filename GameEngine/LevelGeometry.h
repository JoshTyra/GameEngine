#pragma once

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <btBulletDynamicsCommon.h>
#include "Texture.h"
#include "shader.h"
#include "Materials.h"
#include "rendering/Frustum.h"

// Vertex structure
struct Vertex {
    glm::vec3 Position{ 0.0f, 0.0f, 0.0f };
    glm::vec3 Normal{ 0.0f, 0.0f, 0.0f };
    glm::vec2 TexCoords{ 0.0f, 0.0f };
    glm::vec2 LightMapTexCoords{ 0.0f, 0.0f };
    // Initialize other members with appropriate default values
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
    LevelGeometry(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<Texture>& textures);
    void Draw(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection);
    void addTexture(const Texture& texture);
    void setPosition(const glm::vec3& pos) { position = pos; }
    void setRotation(float angle, const glm::vec3& axis) { rotationAngle = angle; rotationAxis = axis; }
    void setScale(const glm::vec3& scl) { scale = scl; }
    btCollisionShape* createBulletCollisionShape() const; // Creates and returns the Bullet collision shape
    void addToPhysicsWorld(btDiscreteDynamicsWorld* dynamicsWorld); // Adds the geometry to the specified Bullet dynamics world

    glm::mat4 getModelMatrix() const;
    glm::vec3 aabbMin;
    glm::vec3 aabbMax;

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

    void setMaterial(std::shared_ptr<Material> mat);

    std::shared_ptr<Material> getMaterial() const {
        return material;
    }

    void calculateAABB();
    bool isInFrustum(const Frustum& frustum) const;

private:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures; // Store textures
    GLuint VAO, VBO, EBO;
    Shader* shader;
    std::shared_ptr<Material> material;
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
    float rotationAngle = 0.0f; // In degrees
    glm::vec3 scale = glm::vec3(1.0f);

    // Optional: Store Bullet physics objects if needed
    // std::unique_ptr<btCollisionShape> collisionShape; // Use smart pointers for automatic memory management
    // std::unique_ptr<btRigidBody> rigidBody;

    void setupMesh();
};