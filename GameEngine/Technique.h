// Technique.h
#pragma once
#include <vector>
#include <string>
#include <GL/glew.h> // For OpenGL constants

struct ShaderInfo {
    std::string type; // "vertex" or "fragment"
    std::string filePath; // Path to the shader file
};

struct BlendingInfo {
    bool enabled = false; // Whether blending is enabled
    GLenum src = GL_SRC_ALPHA; // Source blending factor
    GLenum dest = GL_ONE_MINUS_SRC_ALPHA; // Destination blending factor
    GLenum equation = GL_FUNC_ADD; // Blending equation
};

struct Technique {
    std::vector<ShaderInfo> shaders; // All shaders used in this technique
    bool enableFaceCulling = false; // Default to face culling disabled
    BlendingInfo blending; // Blending state
    bool enableDepthTest;
    GLenum depthFunc;

    // Constructor
    Technique() : enableDepthTest(true), depthFunc(GL_LESS) {}
};
