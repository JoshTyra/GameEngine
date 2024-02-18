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
            material.addTexture(unit, textureName);
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
