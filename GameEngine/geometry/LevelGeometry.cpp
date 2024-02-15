#include "LevelGeometry.h"
#include "shader.h"
#include "Materials.h"
#include "Debug.h"

LevelGeometry::LevelGeometry()
    : VAO(0), VBO(0), EBO(0), shader(nullptr) {
    // Initialize with empty data or default values
}

LevelGeometry::LevelGeometry(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<Texture>& textures)
    : vertices(vertices), indices(indices), textures(textures), VAO(0), VBO(0), EBO(0), shader(nullptr) {
    setupMesh();
    // Apply default transformations
    setPosition(glm::vec3(0.0f)); // Set the initial position if needed
    setScale(glm::vec3(0.25f)); // Scale the model uniformly
    setRotation(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate the model
}

void LevelGeometry::setupMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // Vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    // Vertex normals (if needed in shader)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

    // Vertex texture coords (first UV channel)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    // Assuming LightMapTexCoords is properly set in your Vertex struct
    glEnableVertexAttribArray(3); // Assuming location 3 for lightmap UVs
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, LightMapTexCoords));

    glBindVertexArray(0);
}

void LevelGeometry::Draw(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) {
    if (!shader) {
        std::cerr << "Shader not set for geometry, cannot draw." << std::endl;
        return;
    }

    if (material) {
        const Technique& technique = material->getTechniqueDetails();

        // Check and apply face culling state
        if (technique.enableFaceCulling) {
            glEnable(GL_CULL_FACE);
        }
        else {
            glDisable(GL_CULL_FACE);
        }

        // Apply blending state
        if (technique.blending.enabled) {
            glEnable(GL_BLEND);
            glBlendFunc(technique.blending.src, technique.blending.dest);
            glBlendEquation(technique.blending.equation);
        }
        else {
            glDisable(GL_BLEND);
        }

        if (technique.enableDepthTest) {
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(technique.depthFunc);
        }
        else {
            glDisable(GL_DEPTH_TEST);
        }
    }

    shader->use();
    shader->setMat4("model", model);
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);

    // Initialize texture unit counters for each type of texture
    unsigned int diffuseNr = 0; // Use 0 for the first diffuse texture
    unsigned int emissiveNr = 1; // Use 1 for the first emissive texture, assuming you bind diffuse texture to GL_TEXTURE0

    for (unsigned int i = 0; i < textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i); // Activate proper texture unit before binding
        std::string name = textures[i].type;
        if (name == "diffuse") {
            shader->setInt("diffuseTexture", diffuseNr); // Directly use "diffuseTexture" uniform name
        }
        else if (name == "emissive") {
            shader->setInt("lightMapTexture", emissiveNr); // Directly use "lightMapTexture" uniform name
        }
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    DEBUG_COUT << "Drawing geometry with VAO ID: " << VAO << std::endl;
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0); // Reset active texture unit after binding
}

void LevelGeometry::addTexture(const Texture& texture) {
    textures.push_back(texture);
}

glm::mat4 LevelGeometry::getModelMatrix() const {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position); // Apply translation last
    model = glm::rotate(model, rotationAngle, rotationAxis); // Then apply rotation
    model = glm::scale(model, scale); // Apply scale first
    return model;
}

void LevelGeometry::setMaterial(std::shared_ptr<Material> mat) {
    material = mat;

    // Directly assign the shader pointer from the material's shader program
    shader = material->getShaderProgram();
}



