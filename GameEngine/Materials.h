#pragma once
#include <string>
#include <map>
#include "Technique.h"
#include "shader.h"

class Material {
public:
    Material() = default;

    explicit Material(const std::string& techniqueName);

    const std::string& getTechnique() const;

    const std::map<std::string, std::string>& getTextures() const;

    bool hasTexture(const std::string& unit) const;

    std::string getTexture(const std::string& unit) const;

    void setTechnique(const std::string& techniqueName);

    void addTexture(const std::string& unit, const std::string& textureName);

    void removeTexture(const std::string& unit);

    void setTechniqueDetails(const Technique& techniqueDetails);

    Shader* getShaderProgram() const;

    const Technique& getTechniqueDetails();

    void setShaderProgram(std::unique_ptr<Shader> shader);

private:
    std::string technique;
    std::map<std::string, std::string> textures; // unit -> texture name
    Technique techniqueDetails; // Store detailed technique information
    std::unique_ptr<Shader> shaderProgram;
};