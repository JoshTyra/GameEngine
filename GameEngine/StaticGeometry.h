#pragma once

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <btBulletDynamicsCommon.h>
#include "Texture.h"
#include "shader.h"
#include "Materials.h"
#include "rendering/Frustum.h"
#include "rendering/IRenderable.h"
#include "geometry/StaticVertex.h"
#include "Debug.h"

// StaticGeometry class
class StaticGeometry {
public:
    StaticGeometry();

    StaticGeometry(const std::vector<StaticVertex>& vertices,
        const std::vector<unsigned int>& indices,
        const std::vector<Texture>& textures);

    virtual ~StaticGeometry();
    void draw(const glm::mat4& transform);
    void addTexture(const Texture& texture);
    btCollisionShape* createBulletCollisionShape() const; // Creates and returns the Bullet collision shape
    void addToPhysicsWorld(btDiscreteDynamicsWorld* dynamicsWorld); // Adds the geometry to the specified Bullet dynamics world
    glm::vec3 aabbMin;
    glm::vec3 aabbMax;

    glm::vec3 getAABBMin() const { return aabbMin; }

    glm::vec3 getAABBMax() const { return aabbMax; }

    // Getter function for textures
    const std::vector<Texture>& getTextures() const {
        return this->textures;
    }

    // Assuming textures is a std::vector<Texture> member of StaticGeometry
    unsigned int getDiffuseTextureID() const {
        for (const Texture& texture : textures) {
            if (texture.type == "diffuse") {
                return texture.id;
            }
        }
        return 0; // Return an invalid ID if not found
    }

    // Assuming textures is a std::vector<Texture> member of StaticGeometry
    unsigned int getLightmapTextureID() const {
        for (const Texture& texture : textures) {
            if (texture.type == "emissive") {
                return texture.id;
            }
        }
        return 0; // Return an invalid ID if not found
    }

    // Method to set the shader program
    void setShader(std::shared_ptr<Shader> newShader);

    std::shared_ptr<Shader> getShader() const;

    void setMaterial(std::shared_ptr<Material> mat);

    std::shared_ptr<Material> getMaterial() const {
        return material;
    }

    void calculateAABB();
    bool isInFrustum(const Frustum& frustum) const;

    void setModelMatrix(const glm::mat4& model) { modelMatrix = model; }
    glm::mat4 getModelMatrix() const;
    void updateModelMatrix();

private:
    std::vector<StaticVertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures; // Store textures
    GLuint VAO, VBO, EBO;
    std::shared_ptr<Shader> shader;
    std::shared_ptr<Material> material;
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
    float rotationAngle = 0.0f; // In degrees
    glm::vec3 scale = glm::vec3(1.0f);
    glm::mat4 modelMatrix;

    void setupMesh();
};