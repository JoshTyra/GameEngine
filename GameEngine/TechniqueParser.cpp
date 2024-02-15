#include "TechniqueParser.h"
#include "GLEnumUtils.h" // Make sure this utility is implemented to convert strings to GLenum
#include <tinyxml2.h>
#include <iostream>

Technique TechniqueParser::parseTechniqueXML(const std::string& filename) {
    tinyxml2::XMLDocument doc;
    Technique technique;
    std::cout << "Attempting to load XML file: " << filename << std::endl;
    if (doc.LoadFile(filename.c_str()) != tinyxml2::XML_SUCCESS) {
        std::cerr << "Failed to load XML file: " << filename << std::endl;
        return Technique(); // Return an empty Technique object if file loading fails
    }

    tinyxml2::XMLElement* root = doc.FirstChildElement("technique");
    if (!root) {
        std::cerr << "No 'technique' element found in XML file: " << filename << std::endl;
        return Technique(); // Return an empty Technique object if the root is not found
    }

    std::cout << "Parsing technique from XML file." << std::endl;

    // Parsing each pass (assuming each technique might have multiple passes in the future)
    for (tinyxml2::XMLElement* passElement = root->FirstChildElement("pass"); passElement; passElement = passElement->NextSiblingElement("pass")) {
        std::cout << "Parsing pass." << std::endl;
        // Parsing shaders within each pass
        for (tinyxml2::XMLElement* shaderElement = passElement->FirstChildElement("shader"); shaderElement; shaderElement = shaderElement->NextSiblingElement("shader")) {
            ShaderInfo shader;
            const char* type = shaderElement->Attribute("type");
            const char* file = shaderElement->Attribute("file");
            if (type && file) {
                shader.type = std::string(type);
                shader.filePath = std::string(file);
                technique.shaders.push_back(shader);
                std::cout << "Shader parsed: Type=" << type << ", File=" << file << std::endl;
            }
        }
    }

    // Parsing global render states (not tied to a specific pass)
    tinyxml2::XMLElement* renderStateElement = root->FirstChildElement("renderState");
    while (renderStateElement) {
        const char* name = renderStateElement->Attribute("name");
        if (name) {
            std::string nameStr(name);
            std::cout << "Parsing renderState: " << nameStr << std::endl;
            if (nameStr == "faceCulling") {
                technique.enableFaceCulling = renderStateElement->BoolAttribute("enabled", false);
                std::cout << "Face Culling Enabled: " << std::boolalpha << technique.enableFaceCulling << std::endl;
            }
            else if (nameStr == "depthTest") {
                technique.enableDepthTest = renderStateElement->BoolAttribute("enabled", true);
                std::cout << "Depth Test Enabled: " << std::boolalpha << technique.enableDepthTest << std::endl;
                // Parse depthFunc
                const char* func = renderStateElement->Attribute("func");
                if (func) {
                    // Convert string to GLenum for depthFunc
                    technique.depthFunc = stringToGLEnum(func); // You need to implement stringToGLEnum
                    std::cout << "Depth Func: " << func << std::endl;
                }
            }
            else if (nameStr == "blending") {
                technique.blending.enabled = renderStateElement->BoolAttribute("enabled", false);
                std::cout << "Blending Enabled: " << std::boolalpha << technique.blending.enabled << std::endl;
                tinyxml2::XMLElement* blendFuncElement = renderStateElement->FirstChildElement("blendFunc");
                if (blendFuncElement) {
                    const char* src = blendFuncElement->Attribute("src");
                    const char* dest = blendFuncElement->Attribute("dest");
                    if (src && dest) {
                        // Convert string to GLenum
                        technique.blending.src = stringToGLEnum(src); // Implement stringToGLEnum
                        technique.blending.dest = stringToGLEnum(dest);
                        std::cout << "Blending Func: Src=" << src << ", Dest=" << dest << std::endl;
                    }
                }
                tinyxml2::XMLElement* blendEquationElement = renderStateElement->FirstChildElement("blendEquation");
                if (blendEquationElement) {
                    const char* mode = blendEquationElement->Attribute("mode");
                    if (mode) {
                        // Convert string to GLenum
                        technique.blending.equation = stringToGLEnum(mode);
                        std::cout << "Blending Equation: " << mode << std::endl;
                    }
                }
            }
        }
        // Proceed to the next renderState element
        renderStateElement = renderStateElement->NextSiblingElement("renderState");
    }

    return technique;
}
