#include "LevelGeometry.h"
#include "shader.h" // Include your Shader class header

LevelGeometry::LevelGeometry() {
    // Initialize with empty data or default values
}

LevelGeometry::LevelGeometry(std::vector<Vertex> vertices, std::vector<unsigned int> indices)
    : vertices(vertices), indices(indices) {
    // Setup mesh (VAO, VBO, EBO)
    setupMesh();
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

    shader->use();
    shader->setMat4("model", model);
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);

    // Binding textures and setting uniforms for each texture type
    unsigned int diffuseNr = 1;
    unsigned int emissiveNr = 1;
    for (unsigned int i = 0; i < textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i); // Activate proper texture unit before binding
        std::string number;
        std::string name = textures[i].type;
        if (name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if (name == "texture_emissive")
            number = std::to_string(emissiveNr++);
        std::string uniformName = "material." + name + number;
        shader->setInt(uniformName.c_str(), i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    std::cout << "Drawing geometry with VAO ID: " << VAO << std::endl;
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0); // Reset active texture unit after binding
}


void LevelGeometry::addTexture(const Texture& texture) {
    textures.push_back(texture);
}

glm::mat4 LevelGeometry::getModelMatrix() const {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(rotationAngle), rotationAxis);
    model = glm::scale(model, scale);
    return model;
}
