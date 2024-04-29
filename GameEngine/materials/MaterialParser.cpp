#include "MaterialParser.h"
#include "TechniqueParser.h"
#include "FileSystemUtils.h"
#include <tinyxml2.h>
#include <string>

Material MaterialParser::parseMaterialXML(const std::string& filename) {
    tinyxml2::XMLDocument doc;
    doc.LoadFile(filename.c_str());
    Material material;

    tinyxml2::XMLElement* root = doc.FirstChildElement("material");
    if (root) {
        // Parse technique
        tinyxml2::XMLElement* techniqueElement = root->FirstChildElement("technique");
        if (techniqueElement) {
            std::string techniqueName = techniqueElement->Attribute("name");
            std::string techniqueFilePath = FileSystemUtils::getAssetFilePath("techniques/" + techniqueName);
            Technique technique = TechniqueParser::parseTechniqueXML(techniqueFilePath);
            material.setTechniqueDetails(technique);
        }

        // Parse textures
        for (tinyxml2::XMLElement* textureElement = root->FirstChildElement("texture"); textureElement != nullptr; textureElement = textureElement->NextSiblingElement("texture")) {
            std::string unit = textureElement->Attribute("unit");
            std::string textureName = textureElement->Attribute("name");
            if (unit == "environment") {
                // Parse the XML file that defines the cubemap faces
                std::vector<std::pair<std::string, std::string>> cubemapFaces = parseCubemapXML(textureName);
                material.setCubemapFaces(cubemapFaces);
                material.addTexture(unit, textureName);
            }
            else {
                material.addTexture(unit, textureName);
            }
        }

        // Parse parameters
        for (tinyxml2::XMLElement* parameterElement = root->FirstChildElement("parameter"); parameterElement != nullptr; parameterElement = parameterElement->NextSiblingElement("parameter")) {
            std::string name = parameterElement->Attribute("name");
            float value = parameterElement->FloatAttribute("value");
            material.addParameter(name, value);
        }
    }

    return material;
}

// Helper function to parse the cubemap XML and extract face paths
std::vector<std::pair<std::string, std::string>> MaterialParser::parseCubemapXML(const std::string& filename) {
    tinyxml2::XMLDocument doc;
    std::vector<std::pair<std::string, std::string>> facePaths;
    doc.LoadFile(FileSystemUtils::getAssetFilePath("textures/" + filename).c_str());

    if (doc.LoadFile(FileSystemUtils::getAssetFilePath("textures/" + filename).c_str()) != tinyxml2::XML_SUCCESS) {
        std::cerr << "Failed to load cubemap XML file: " << filename << std::endl;
        return facePaths;
    }

    tinyxml2::XMLElement* root = doc.FirstChildElement("cubemap");
    if (root) {
        const char* faceNames[] = { "right", "left", "top", "bottom", "front", "back" };
        for (auto& faceName : faceNames) {
            tinyxml2::XMLElement* faceElement = root->FirstChildElement(faceName);
            if (faceElement) {
                std::string path = faceElement->Attribute("path");
                facePaths.emplace_back(faceName, path);
            }
        }
    }

    return facePaths;
}
