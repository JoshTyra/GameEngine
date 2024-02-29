#pragma once

#include <GL/glew.h>
#include "Shader.h" 
#include "ScreenQuad.h"
#include <string>
#include <map>

class PostProcessing {
public:
    PostProcessing();
    ~PostProcessing();
    void addEffect(const std::string& name, const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
    void setActiveEffect(const std::string& name);
    void applyActiveEffect(GLuint inputTexture);

private:
    std::map<std::string, Shader> shaders;
    Shader* activeShader = nullptr;  // Pointer to the currently active shader
    ScreenQuad screenQuad;
};
