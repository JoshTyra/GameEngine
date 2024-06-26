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

const std::vector<std::pair<std::string, std::string>>& Material::getCubemapFaces() const {
    return cubemapFaces;
}

bool Material::hasCubemapFaces() const {
    return !cubemapFaces.empty();
}

void Material::setCubemapFaces(const std::vector<std::pair<std::string, std::string>>& faces) {
    cubemapFaces = faces;
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

    // Use std::make_shared for allocation
    shaderProgram = std::make_shared<Shader>(vertexShaderPath, fragmentShaderPath);
    if (!shaderProgram->isProgramLinkedSuccessfully()) {
        std::cerr << "Failed to compile/link shader program for material." << std::endl;
    }
}

std::shared_ptr<Shader> Material::getShaderProgram() const {
    return shaderProgram; // Simply return the shared_ptr
}

void Material::setShaderProgram(std::shared_ptr<Shader> shader) {
    shaderProgram = shader; // Direct assignment thanks to shared_ptr
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

const std::map<std::string, std::string> Material::textureUniformMap = {
    {"diffuse", "textures[0]"},
    {"emissive", "textures[1]"},
    {"detail1", "textures[2]"},
    {"detail2", "textures[3]"},
    {"normal", "textures[4]"},
    {"environment", "environmentMap"} // Use a separate uniform for the cubemap
};


