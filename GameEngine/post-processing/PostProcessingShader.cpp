#include "PostProcessingShader.h"
#include "FileSystemUtils.h"

PostProcessing::PostProcessing() : activeShader(nullptr) {
}

PostProcessing::~PostProcessing() {
    // Cleanup resources if necessary
}

void PostProcessing::applyActiveEffect(GLuint inputTexture) {
    if (activeShader) {
        activeShader->use();
        activeShader->setInt("screenTexture", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, inputTexture);
        screenQuad.render();
        glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture after use
    }
    else {
        std::cerr << "No active post-processing effect set." << std::endl;
    }
}

void PostProcessing::addEffect(const std::string& name, const std::string& vertexShaderPath, const std::string& fragmentShaderPath) {
    shaders.try_emplace(name, vertexShaderPath, fragmentShaderPath);
}

void PostProcessing::setActiveEffect(const std::string& name) {
    auto found = shaders.find(name);
    if (found != shaders.end()) {
        activeShader = &found->second;
        assert(activeShader != nullptr);
    }
    else {
        std::cerr << "Shader not found: " << name << std::endl;
        activeShader = nullptr;
    }
}


