#pragma once
#include <string>
#include <map>
#include <memory>
#include "Technique.h"
#include "shader.h"
#include "TextureLoader.h"

class Material {
public:
    Material() = default;
    explicit Material(const std::string& techniqueName);

    const std::string& getTechnique() const;
    const std::map<std::string, std::string>& getTextures() const;
    const std::vector<std::pair<std::string, std::string>>& getCubemapFaces() const;  // Function to retrieve cubemap faces
    bool hasTexture(const std::string& unit) const;
    bool hasCubemapFaces() const;  // Check if cubemap faces are present
    std::string getTexture(const std::string& unit) const;

    const std::string& getName() const;
    void setName(const std::string& materialName);
    void setTechnique(const std::string& techniqueName);
    void addTexture(const std::string& unit, const std::string& textureName);
    void setCubemapFaces(const std::vector<std::pair<std::string, std::string>>& faces);  // Add or set cubemap faces
    void removeTexture(const std::string& unit);

    void setTechniqueDetails(const Technique& techniqueDetails);
    std::shared_ptr<Shader> getShaderProgram() const;
    const Technique& getTechniqueDetails() const;

    void setShaderProgram(std::shared_ptr<Shader> shader);
    void addParameter(const std::string& name, float value);
    float getParameter(const std::string& name) const;
    bool hasParameter(const std::string& name) const;

    static const std::map<std::string, std::string> textureUniformMap;

private:
    std::string name;
    std::string technique;
    std::map<std::string, std::string> textures; // unit -> texture name
    std::vector<std::pair<std::string, std::string>> cubemapFaces; // face identifier -> texture path
    std::map<std::string, float> parameters;
    Technique techniqueDetails; // Store detailed technique information
    std::shared_ptr<Shader> shaderProgram;
};