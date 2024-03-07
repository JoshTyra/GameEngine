#include "PostProcessing.h"

PostProcessing::PostProcessing() {
    // Constructor implementation
    // Initialize any members here if necessary
}

PostProcessing::~PostProcessing() {
    // Clean up resources if necessary
}

void PostProcessing::addEffect(const std::string& name, const std::string& vertexShaderPath, const std::string& fragmentShaderPath) {
    // Load and compile the shader
    Shader shader(vertexShaderPath, fragmentShaderPath);
    PostProcessingEffect effect(shader);
    // Initialize default uniforms for the effect here (if any)

    // Store the effect using its name as the key
    effects.emplace(name, std::move(effect));
}

void PostProcessing::applyEffects(GLuint inputTexture) {
    GLuint currentInputTexture = inputTexture;

    // Assuming the screenQuad has been initialized elsewhere (e.g., in the constructor)
    for (const auto& effectName : activeEffects) {
        auto it = effects.find(effectName);
        if (it != effects.end()) {
            PostProcessingEffect& effect = it->second; // Correct, assuming 'it' is valid

            effect.shader.use();
            effect.shader.setInt("screenTexture", 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, currentInputTexture);

            // Apply each uniform set for the effect
            for (const auto& uniform : effect.uniforms) {
                uniform.applyToShader(effect.shader);
            }

            screenQuad.render();

            // Here, you'd handle ping-ponging between framebuffers if necessary
            // and update currentInputTexture accordingly
        }
    }
}

void PostProcessing::setActiveEffects(const std::vector<std::string>& effectNames) {
    activeEffects = effectNames;  // Directly store the names
}
