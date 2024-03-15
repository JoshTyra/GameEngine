#include "Shader.h"

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath) {
    std::string vertexShaderSource, fragmentShaderSource;
    if (!loadShaderFromFile(vertexPath, vertexShaderSource) ||
        !loadShaderFromFile(fragmentPath, fragmentShaderSource)) {
        Program = 0;
        return;
    }

    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    Program = glCreateProgram();
    glAttachShader(Program, vertexShader);
    glAttachShader(Program, fragmentShader);
    glLinkProgram(Program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    if (!checkProgramLinkStatus(Program)) {
        glDeleteProgram(Program);
        Program = 0;
    }
}

Shader::~Shader() {
    if (Program) {
        glDeleteProgram(Program);
    }
}

void Shader::use() const {
    if (isValid()) {
        glUseProgram(Program);
    }
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat) const {
    if (isValid()) {
        GLint location = getUniformLocation(name);
        if (location != -1) {
            glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
        }
    }
}

void Shader::setInt(const std::string& name, int value) const {
    if (isValid()) {
        GLint location = getUniformLocation(name);
        if (location != -1) {
            glUniform1i(location, value);
        }
    }
}

void Shader::setFloat(const std::string& name, float value) const {
    if (isValid()) {
        GLint location = getUniformLocation(name);
        if (location != -1) {
            glUniform1f(location, value);
        }
    }
}

void Shader::setVec2(const std::string& name, const glm::vec2& value) const {
    if (isValid()) {
        GLint location = getUniformLocation(name);
        if (location != -1) {
            glUniform2fv(location, 1, &value[0]);
        }
    }
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const {
    if (isValid()) {
        GLint location = getUniformLocation(name);
        if (location != -1) {
            glUniform3fv(location, 1, &value[0]);
        }
    }
}

bool Shader::loadShaderFromFile(const std::string& filePath, std::string& shaderSource) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open shader file: " << filePath << std::endl;
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    shaderSource = buffer.str();
    return true;
}

GLuint Shader::compileShader(GLenum shaderType, const std::string& shaderSource) {
    GLuint shader = glCreateShader(shaderType);
    const char* source = shaderSource.c_str();
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    if (!checkShaderCompilationStatus(shader)) {
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

bool Shader::checkShaderCompilationStatus(GLuint shader) {
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        GLint infoLogLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

        std::vector<GLchar> infoLog(infoLogLength);
        glGetShaderInfoLog(shader, infoLogLength, nullptr, infoLog.data());

        std::cerr << "Shader compilation failed: " << infoLog.data() << std::endl;
        return false;
    }

    return true;
}

bool Shader::checkProgramLinkStatus(GLuint program) {
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status != GL_TRUE) {
        GLint infoLogLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

        std::vector<GLchar> infoLog(infoLogLength);
        glGetProgramInfoLog(program, infoLogLength, nullptr, infoLog.data());

        std::cerr << "Program link failed: " << infoLog.data() << std::endl;
        return false;
    }

    return true;
}

GLint Shader::getUniformLocation(const std::string& name) const {
    auto it = uniformLocations.find(name);
    if (it != uniformLocations.end()) {
        return it->second;
    }

    GLint location = glGetUniformLocation(Program, name.c_str());
    uniformLocations[name] = location;
    return location;
}