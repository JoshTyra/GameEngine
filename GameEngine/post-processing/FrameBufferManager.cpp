#include "FrameBufferManager.h"

FrameBufferManager::FrameBufferManager(GLFWwindow* window) : window(window) {
    createPostProcessingEffects();
}

FrameBufferManager::~FrameBufferManager() {
    // Clean up frame buffers and related resources
    for (const auto& frameBuffer : frameBuffers) {
        glDeleteFramebuffers(1, &frameBuffer.frameBufferId);
        glDeleteTextures(1, &frameBuffer.textureId);
        glDeleteRenderbuffers(1, &frameBuffer.renderBufferId);
    }
}

void FrameBufferManager::createFrameBuffer(int width, int height) {
    screenWidth = width;
    screenHeight = height;

    FrameBuffer frameBuffer;

    // Generate and bind the framebuffer
    glGenFramebuffers(1, &frameBuffer.frameBufferId);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer.frameBufferId);

    // Generate the texture and attach it to the framebuffer
    glGenTextures(1, &frameBuffer.textureId);
    glBindTexture(GL_TEXTURE_2D, frameBuffer.textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Use mipmaps for minification
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Linear filter for magnification

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameBuffer.textureId, 0);

    // Generate and attach the renderbuffer for depth
    glGenRenderbuffers(1, &frameBuffer.renderBufferId);
    glBindRenderbuffer(GL_RENDERBUFFER, frameBuffer.renderBufferId);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, screenWidth, screenHeight);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, frameBuffer.renderBufferId);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

    // Unbind the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    frameBuffers.push_back(frameBuffer);
}

void FrameBufferManager::bindFrameBuffer(unsigned int index) {
    if (index < frameBuffers.size()) {
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffers[index].frameBufferId);
    }
}

void FrameBufferManager::unbindFrameBuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBufferManager::createPostProcessingEffects() {
    // BrightPass shader
    Shader brightPassShader(FileSystemUtils::getAssetFilePath("shaders/invert.vert"),
        FileSystemUtils::getAssetFilePath("shaders/bright_pass.frag"));
    PostProcessingEffect brightPassEffect(std::move(brightPassShader));
    brightPassEffect.addUniform(ShaderUniform("brightnessThreshold", 1.0f));
    postProcessing.addEffect("brightPass", std::move(brightPassEffect));

    // DownSample shader
    Shader downSampleShader(FileSystemUtils::getAssetFilePath("shaders/invert.vert"),
        FileSystemUtils::getAssetFilePath("shaders/downsample.frag"));
    PostProcessingEffect downSampleEffect(std::move(downSampleShader));
    downSampleEffect.addUniform(ShaderUniform("resolutionFactor", 0.5f)); // Half-size downsample
    //downSampleEffect.addUniform(ShaderUniform("lod", 1.0f)); // Half-size downsample
    postProcessing.addEffect("downSample", std::move(downSampleEffect));

    // HorizontalBlur shader
    Shader horizontalBlurShader(FileSystemUtils::getAssetFilePath("shaders/invert.vert"),
        FileSystemUtils::getAssetFilePath("shaders/horizontalBlur.frag"));
    PostProcessingEffect horizontalBlurEffect(std::move(horizontalBlurShader));
    horizontalBlurEffect.addUniform(ShaderUniform("blurSize", 1.0f)); // Adjust the blur size as needed
    postProcessing.addEffect("horizontalBlur", std::move(horizontalBlurEffect));

    // VerticalBlur shader
    Shader verticalBlurShader(FileSystemUtils::getAssetFilePath("shaders/invert.vert"),
        FileSystemUtils::getAssetFilePath("shaders/verticalBlur.frag"));
    PostProcessingEffect verticalBlurEffect(std::move(verticalBlurShader));
    verticalBlurEffect.addUniform(ShaderUniform("blurSize", 1.0f)); // Adjust the blur size as needed
    postProcessing.addEffect("verticalBlur", std::move(verticalBlurEffect));

    // UpSample shader
    Shader upSampleShader(FileSystemUtils::getAssetFilePath("shaders/invert.vert"),
        FileSystemUtils::getAssetFilePath("shaders/upscale.frag"));
    PostProcessingEffect upSampleEffect(std::move(upSampleShader));
    //upSampleEffect.addUniform(ShaderUniform("textureSize", glm::vec2(1280.0f, 530.0f)));
    //upSampleEffect.addUniform(ShaderUniform("inverseTextureSize", glm::vec2(1.0f / 1280.0f, 1.0f / 530.0f)));
    postProcessing.addEffect("upSample", std::move(upSampleEffect));

    // Color Grading shader
    Shader colorGradingShader(FileSystemUtils::getAssetFilePath("shaders/invert.vert"),
        FileSystemUtils::getAssetFilePath("shaders/colorGrading.frag"));
    PostProcessingEffect colorGradingEffect(std::move(colorGradingShader));
    colorGradingEffect.addUniform(ShaderUniform("saturation", 1.5f)); // Example: Increase saturation
    colorGradingEffect.addUniform(ShaderUniform("contrast", 1.1f)); // Example: Slightly increase contrast
    colorGradingEffect.addUniform(ShaderUniform("tint", glm::vec3(0.05f, 0.05f, 0.05f))); // Example: Apply a slight tint
    postProcessing.addEffect("colorGrading", std::move(colorGradingEffect));

    // BloomFinal shader
    Shader bloomFinalShader(FileSystemUtils::getAssetFilePath("shaders/invert.vert"),
        FileSystemUtils::getAssetFilePath("shaders/bloom_final.frag"));
    PostProcessingEffect bloomFinalEffect(std::move(bloomFinalShader));
    bloomFinalEffect.addUniform(ShaderUniform("bloomIntensity", 1.25f));
    postProcessing.addEffect("bloomFinal", std::move(bloomFinalEffect));

    activeEffects = { "brightPass", "downSample", "horizontalBlur", "verticalBlur", "upSample", "colorGrading", "bloomFinal" };
    postProcessing.setActiveEffects(activeEffects);
}

void FrameBufferManager::applyPostProcessingEffects(GLuint inputTexture) {

    // 1. Apply the bright pass effect at full resolution
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffers[1].frameBufferId);
    glViewport(0, 0, screenWidth, screenHeight); // Set viewport to match full resolution
    postProcessing.applyEffect("brightPass", frameBuffers[0].textureId, 0);

    // 2. Apply the downsample effect at half resolution
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffers[2].frameBufferId);
    glViewport(0, 0, screenWidth / 2, screenHeight / 2); // Set viewport for half resolution
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    GLuint brightPassResultTexture = frameBuffers[1].textureId;
    postProcessing.applyEffect("downSample", brightPassResultTexture, 0);

    // Replace Kawase blur with Gaussian blur in applyPostProcessingEffects
    GLuint currentInputTexture = frameBuffers[2].textureId;

    // Apply horizontal blur effect
    GLuint horizontalBlurFBO = frameBuffers[3].frameBufferId; // Use an intermediate framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, horizontalBlurFBO);
    glViewport(0, 0, screenWidth / 2, screenHeight / 2); // Adjust viewport if needed
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    postProcessing.applyEffect("horizontalBlur", currentInputTexture, 0);

    // Apply vertical blur effect
    GLuint verticalBlurFBO = frameBuffers[4].frameBufferId; // Use another framebuffer to complete the blur
    glBindFramebuffer(GL_FRAMEBUFFER, verticalBlurFBO);
    glViewport(0, 0, screenWidth / 2, screenHeight / 2); // Adjust viewport if needed
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    postProcessing.applyEffect("verticalBlur", frameBuffers[3].textureId, 0);

    currentInputTexture = frameBuffers[4].textureId; // The texture ID of the framebuffer with the completed blur

    // 4. Apply the upSample effect and render to full resolution
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffers[5].frameBufferId);
    glViewport(0, 0, screenWidth, screenHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    postProcessing.applyEffect("upSample", currentInputTexture, 0);

    // Assuming frameBuffers[6] is allocated for color grading effect
    // You might need to create/setup this framebuffer if not done already
    GLuint colorGradingFBO = frameBuffers[6].frameBufferId;
    glBindFramebuffer(GL_FRAMEBUFFER, colorGradingFBO);
    glViewport(0, 0, screenWidth, screenHeight); // Match full resolution for color grading
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Apply color grading to the upsampled texture (now in frameBuffers[5])
    postProcessing.applyEffect("colorGrading", frameBuffers[5].textureId, 0);

    // 5. Apply the bloomFinal effect, combining the original scene and the processed bloom
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Render to the default framebuffer (screen)
    glViewport(0, 0, screenWidth, screenHeight); // Ensure viewport matches screen resolution for final output
    // Use the color-graded bloom texture (now in frameBuffers[6]) for the final combination
    postProcessing.applyEffect("bloomFinal", frameBuffers[0].textureId, frameBuffers[6].textureId);
}

void FrameBufferManager::setActiveEffects(const std::vector<std::string>& effectNames) {
    activeEffects = effectNames;
}

GLuint FrameBufferManager::getSceneTexture() {
    if (!frameBuffers.empty()) {
        return frameBuffers[0].textureId; // Return the texture ID of the first framebuffer
    }
    return 0; // Return an invalid ID if there are no framebuffers
}