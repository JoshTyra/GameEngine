#include "Materials.h"
#include "FileSystemUtils.h"

Material::Material(const std::string& techniqueName) : technique(techniqueName) {}

const std::string& Material::getTechnique() const {
    return technique;
}

const std::map<std::string, std::string>& Material::getTextures() const {
    return textures;
}

bool Material::hasTexture(const std::string& unit) const {
    return textures.find(unit) != textures.end();
}

std::string Material::getTexture(const std::string& unit) const {
    auto it = textures.find(unit);
    if (it != textures.end()) {
        return it->second;
    }
    return "";
}

void Material::setTechnique(const std::string& techniqueName) {
    technique = techniqueName;
}

void Material::addTexture(const std::string& unit, const std::string& textureName) {
    textures[unit] = textureName;
}

void Material::removeTexture(const std::string& unit) {
    textures.erase(unit);
}

void Material::setTechniqueDetails(const Technique& techniqueDetails) {
    this->techniqueDetails = techniqueDetails;
    std::string vertexShaderPath, fragmentShaderPath;
    for (const auto& shaderInfo : techniqueDetails.shaders) {
        std::string fullPath = FileSystemUtils::getAssetFilePath(shaderInfo.filePath);
        if (shaderInfo.type == "vertex") {
            vertexShaderPath = fullPath;
        }
        else if (shaderInfo.type == "fragment") {
            fragmentShaderPath = fullPath;
        }
    }
    shaderProgram = std::make_unique<Shader>(vertexShaderPath, fragmentShaderPath);
    if (!shaderProgram->isProgramLinkedSuccessfully()) {
        std::cerr << "Failed to compile/link shader program for material." << std::endl;
    }
}

Shader* Material::getShaderProgram() const {
    return shaderProgram.get();
}

void Material::setShaderProgram(std::unique_ptr<Shader> shader) {
    shaderProgram = std::move(shader);
}

const Technique& Material::getTechniqueDetails() const {
    return this->techniqueDetails;
}

void Material::addParameter(const std::string& name, float value) {
    parameters[name] = value;
}

float Material::getParameter(const std::string& name) const {
    auto it = parameters.find(name);
    return it != parameters.end() ? it->second : 0.0f; // Return 0 if not found
}

bool Material::hasParameter(const std::string& name) const {
    return parameters.find(name) != parameters.end();
}

// Initialize the static map
const std::map<std::string, std::string> Material::textureUniformMap = {
    {"diffuse", "textures[0]"},
    {"emissive", "textures[1]"},
    {"detail1", "textures[2]"},
    {"detail2", "textures[3]"}
};


