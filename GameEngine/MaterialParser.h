#pragma once

#include "Materials.h"  // Assuming Material class is defined in this header
#include <string>

class MaterialParser {
public:
    static Material parseMaterialXML(const std::string& filename);
    static std::map<std::string, std::string> parseCubemapXML(const std::string& filename);
};