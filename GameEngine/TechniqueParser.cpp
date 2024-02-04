// TechniqueParser.cpp
#include "TechniqueParser.h"
#include <tinyxml2.h>

Technique TechniqueParser::parseTechniqueXML(const std::string& filename) {
    tinyxml2::XMLDocument doc;
    Technique technique;
    doc.LoadFile(filename.c_str());

    tinyxml2::XMLElement* root = doc.FirstChildElement("technique");
    if (root) {
        for (tinyxml2::XMLElement* passElement = root->FirstChildElement("pass"); passElement != nullptr; passElement = passElement->NextSiblingElement("pass")) {
            for (tinyxml2::XMLElement* shaderElement = passElement->FirstChildElement("shader"); shaderElement != nullptr; shaderElement = shaderElement->NextSiblingElement("shader")) {
                ShaderInfo shader;
                shader.type = shaderElement->Attribute("type");
                shader.filePath = shaderElement->Attribute("file");
                technique.shaders.push_back(shader);
            }
            // Since your current structure does not support multiple passes explicitly,
            // the loop adds all shaders to the technique. Adjust if multi-pass support is added.
        }
    }

    return technique;
}

