#include "PostProcessing.h"

PostProcessing::PostProcessing() {
    // Constructor implementation
    // Initialize any members here if necessary
}

PostProcessing::~PostProcessing() {
    // Clean up resources if necessary
}

void PostProcessing::addEffect(const std::string& name, PostProcessingEffect&& effect) {
    effects.emplace(std::piecewise_construct,
        std::forward_as_tuple(name),
        std::forward_as_tuple(std::move(effect)));
}

void PostProcessing::setActiveEffects(const std::vector<std::string>& effectNames) {
    activeEffects = effectNames;  // Directly store the names
}

void PostProcessing::applyEffect(const std::string& effectName, GLuint inputTexture1, GLuint inputTexture2) {
    auto it = effects.find(effectName);
    if (it == effects.end()) {
        std::cerr << "Effect '" << effectName << "' not found." << std::endl;
        return;
    }

    // Access the effect by reference to avoid copying the Shader object
    PostProcessingEffect& effect = it->second;

    // Now use effect.shader without copying the Shader object
    effect.shader.use();

    // Bind the input texture(s)
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, inputTexture1);
    effect.shader.setInt("sceneTexture", 0); // Corrected to use effect.shader

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, inputTexture2);
    effect.shader.setInt("bloomBlurTexture", 1); // Corrected to use effect.shader

    // Set the effect's uniforms
    effect.applyUniforms();

    // Draw a full-screen quad to apply the effect
    screenQuad.render();

    // Optionally, reset active texture unit to a default state if needed
    glActiveTexture(GL_TEXTURE0);
}

void PostProcessing::updateUniform(const std::string& effectName, const std::string& uniformName, const float value) {
    auto it = effects.find(effectName);
    if (it != effects.end()) {
        PostProcessingEffect& effect = it->second;
        effect.shader.use();
        effect.shader.setFloat(uniformName, value);
    }
}

void PostProcessing::updateUniform(const std::string& effectName, const std::string& uniformName, const glm::vec2& value) {
    auto it = effects.find(effectName);
    if (it != effects.end()) {
        PostProcessingEffect& effect = it->second;
        effect.shader.use();
        effect.shader.setVec2(uniformName, value);
    }
}

void PostProcessing::updateUniform(const std::string& effectName, const std::string& uniformName, const glm::vec3& value) {
    auto it = effects.find(effectName);
    if (it != effects.end()) {
        PostProcessingEffect& effect = it->second;
        effect.shader.use();
        effect.shader.setVec3(uniformName, value);
    }
}

void PostProcessing::updateUniform(const std::string& effectName, const std::string& uniformName, const glm::vec4& value) {
    auto it = effects.find(effectName);
    if (it != effects.end()) {
        PostProcessingEffect& effect = it->second;
        effect.shader.use();
        effect.shader.setVec4(uniformName, value);
    }
}
