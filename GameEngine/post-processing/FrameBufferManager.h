#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <glm/glm.hpp>
#include "ScreenQuad.h"
#include "FileSystemUtils.h"
#include "shader.h"
#include "post-processing/PostProcessing.h"

class FrameBufferManager {
public:
    FrameBufferManager(GLFWwindow* window);
    ~FrameBufferManager();

    void createFrameBuffer(int width, int height);
    void bindFrameBuffer(unsigned int index);
    void unbindFrameBuffer();

    void createPostProcessingEffects();
    void applyPostProcessingEffects(GLuint inputTexture);
    void setActiveEffects(const std::vector<std::string>& effectNames);
    GLuint getSceneTexture();

private:
    struct FrameBuffer {
        GLuint frameBufferId;
        GLuint textureId;
        GLuint renderBufferId;
    };

    std::vector<FrameBuffer> frameBuffers;
    int screenWidth, screenHeight;
    std::vector<std::string> activeEffects;
    PostProcessing postProcessing;
    ScreenQuad screenQuad;
    GLFWwindow* window;
};