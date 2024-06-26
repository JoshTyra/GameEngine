#pragma once

#include <GL/glew.h>
#include <vector>
#include <string>
#include "Shader.h" // Adjust the path if necessary
#include "FileSystemUtils.h"
#include <glm/glm.hpp>
#include <node/Node.h>

class SkyboxNode : public Node {
public:
    SkyboxNode(const std::vector<std::string>& faces);
    ~SkyboxNode();
    void draw(const glm::mat4& view, const glm::mat4& projection) const;
    void updateCubemap(const std::vector<std::string>& faces); // For dynamic skybox updates

private:
    GLuint VAO, VBO, textureID;
    std::unique_ptr<Shader> skyboxShader; // Use std::unique_ptr for the shader
    static GLfloat skyboxVertices[108]; // Declared as static
    void setupSkybox();
    GLuint loadCubemap(const std::vector<std::string>& faces);
};
