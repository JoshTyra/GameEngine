#include "GLEnumUtils.h"
#include <unordered_map>
#include <iostream>

GLenum stringToGLEnum(const std::string& str) {
    static const std::unordered_map<std::string, GLenum> strToEnumMap = {
        // Add all mappings here as shown previously
    };

    auto it = strToEnumMap.find(str);
    if (it != strToEnumMap.end()) {
        return it->second;
    }

    std::cerr << "Unknown OpenGL enum string: " << str << std::endl;
    return 0; // GL_ZERO as a default or error value
}
