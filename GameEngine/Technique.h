// Technique.h
#pragma once
#include <vector>
#include <string>

struct ShaderInfo {
    std::string type; // "vertex" or "fragment"
    std::string filePath; // Path to the shader file
};

struct Technique {
    std::vector<ShaderInfo> shaders; // All shaders used in this technique
    // Add other technique-related properties here
};
