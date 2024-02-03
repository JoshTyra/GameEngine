#pragma once
#include <string>
#include <map>
#include "Technique.h"

class Material {
public:
    // Default constructor
    Material() {}

    // Constructor with technique
    explicit Material(const std::string& techniqueName)
        : technique(techniqueName) {}

    // Getters
    const std::string& getTechnique() const {
        return technique;
    }

    const std::map<std::string, std::string>& getTextures() const {
        return textures;
    }

    // Check if a texture exists for a given unit
    bool hasTexture(const std::string& unit) const {
        return textures.find(unit) != textures.end();
    }

    // Get texture by unit
    std::string getTexture(const std::string& unit) const {
        auto it = textures.find(unit);
        if (it != textures.end()) {
            return it->second;
        }
        return ""; // Return empty string if not found
    }

    // Setters
    void setTechnique(const std::string& techniqueName) {
        technique = techniqueName;
    }

    void addTexture(const std::string& unit, const std::string& textureName) {
        textures[unit] = textureName;
    }

    // Remove a texture by unit
    void removeTexture(const std::string& unit) {
        textures.erase(unit);
    }

    // Method to set technique details
    void setTechniqueDetails(const Technique& techniqueDetails) {
        this->techniqueDetails = techniqueDetails;
        // You might also compile and link shaders here or store shader paths for later use
    }

    // Method to retrieve technique details
    const Technique& getTechniqueDetails() const {
        return techniqueDetails;
    }

private:
    std::string technique;
    std::map<std::string, std::string> textures; // unit -> texture name
    Technique techniqueDetails; // Store detailed technique information
};