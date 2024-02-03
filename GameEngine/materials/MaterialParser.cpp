#include "MaterialParser.h"
#include "TechniqueParser.h"
#include "FileSystemUtils.h"
#include <tinyxml2.h>

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
            // Use TechniqueParser to parse the technique file
            std::string techniqueFilePath = FileSystemUtils::getAssetFilePath("techniques/" + techniqueName);
            Technique technique = TechniqueParser::parseTechniqueXML(techniqueFilePath);
            material.setTechniqueDetails(technique); // Assuming Material can store Technique details
        }

        // Parse textures
        for (tinyxml2::XMLElement* textureElement = root->FirstChildElement("texture"); textureElement != nullptr; textureElement = textureElement->NextSiblingElement("texture")) {
            std::string unit = textureElement->Attribute("unit");
            std::string textureName = textureElement->Attribute("name");
            material.addTexture(unit, textureName);
        }
    }

    return material;
}
