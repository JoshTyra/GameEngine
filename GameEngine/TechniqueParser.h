// TechniqueParser.h
#pragma once
#include <string>
#include "Technique.h" // Assume this header defines the Technique struct

class TechniqueParser {
public:
    static Technique parseTechniqueXML(const std::string& filename);
};
