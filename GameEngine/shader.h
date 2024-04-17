#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
public:
    GLuint Program;

    // Constructor: unchanged
    Shader(const std::string& vertexPath, const std::string& fragmentPath);

    // Destructor
    ~Shader();

    // Delete copy constructor and copy assignment operator to prevent copying
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    // Implement move constructor
    Shader(Shader&& other) noexcept : Program(other.Program) {
        other.Program = 0; // Transfer ownership and prevent deletion by moved-from object
    }

    // Implement move assignment operator
    Shader& operator=(Shader&& other) noexcept {
        if (this != &other) {
            if (Program != 0) {
                glDeleteProgram(Program);
            }
            Program = other.Program;
            other.Program = 0; // Transfer ownership and prevent deletion by moved-from object
        }
        return *this;
    }

    void use() const;
    void setMat4(const std::string& name, const glm::mat4& mat) const;
    void setMat3(const std::string& name, const glm::mat3& mat) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec2(const std::string& name, const glm::vec2& value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec4(const std::string& name, const glm::vec4& value) const;

    bool hasUniform(const std::string& name) const;
    bool isProgramLinkedSuccessfully() const;

private:
    void checkCompileErrors(GLuint shader, std::string type);
};
