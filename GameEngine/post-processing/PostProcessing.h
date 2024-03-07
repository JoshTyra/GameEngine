#pragma once

#include "Shader.h"
#include "ScreenQuad.h"
#include <GL/glew.h>
#include <vector>
#include <unordered_map>
#include <string>
#include <glm/glm.hpp>

struct ShaderUniform {
    std::string name;
    enum class Type { Int, Float, Vec2, Vec3, Mat4 } type;
    union {
        int intValue;
        float floatValue;
        glm::vec2 vec2Value;
        glm::vec3 vec3Value;
        glm::mat4 mat4Value;
    };

    ShaderUniform(const std::string& name, int value) : name(name), type(Type::Int), intValue(value) {}
    ShaderUniform(const std::string& name, float value) : name(name), type(Type::Float), floatValue(value) {}
    ShaderUniform(const std::string& name, glm::vec2 value) : name(name), type(Type::Vec2), vec2Value(value) {}
    ShaderUniform(const std::string& name, glm::vec3 value) : name(name), type(Type::Vec3), vec3Value(value) {}
    ShaderUniform(const std::string& name, glm::mat4 value) : name(name), type(Type::Mat4), mat4Value(value) {}

    void applyToShader(const Shader& shader) const {
        switch (type) {
        case Type::Int: shader.setInt(name, intValue); break;
        case Type::Float: shader.setFloat(name, floatValue); break;
        case Type::Vec2: shader.setVec2(name, vec2Value); break;
        case Type::Vec3: shader.setVec3(name, vec3Value); break;
        case Type::Mat4: shader.setMat4(name, mat4Value); break;
        }
    }
};

struct PostProcessingEffect {
    Shader shader; // Assumes Shader class is accessible and contains methods like setInt, setFloat, etc.
    std::vector<ShaderUniform> uniforms;

    PostProcessingEffect(const Shader& shader) : shader(shader) {}

    void addUniform(const ShaderUniform& uniform) {
        uniforms.push_back(uniform);
    }

    void applyUniforms() const {
        for (const auto& uniform : uniforms) {
            uniform.applyToShader(shader);
        }
    }
};

class PostProcessing {
public:
    PostProcessing();
    ~PostProcessing();
    void applyEffects(GLuint inputTexture);
    void addEffect(const std::string& name, PostProcessingEffect&& effect);
    void setActiveEffects(const std::vector<std::string>& effectNames);

private:
    std::unordered_map<std::string, PostProcessingEffect> effects;
    std::vector<std::string> activeEffects;
    ScreenQuad screenQuad;
    // Additional members as needed
};
