#include "FrameBufferManager.h"

FrameBufferManager::FrameBufferManager() {
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameBuffer.textureId, 0);

    // Generate and attach the renderbuffer for depth
    glGenRenderbuffers(1, &frameBuffer.renderBufferId);
    glBindRenderbuffer(GL_RENDERBUFFER, frameBuffer.renderBufferId);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, screenWidth, screenHeight);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, frameBuffer.renderBufferId);

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
    Shader originalPassShader(FileSystemUtils::getAssetFilePath("shaders/invert.vert"),
        FileSystemUtils::getAssetFilePath("shaders/originalScene.frag"));
    PostProcessingEffect originalPassEffect(std::move(originalPassShader));
    postProcessing.addEffect("originalScenePass", std::move(originalPassEffect));

    // BrightPass shader
    Shader brightPassShader(FileSystemUtils::getAssetFilePath("shaders/invert.vert"),
        FileSystemUtils::getAssetFilePath("shaders/bright_pass.frag"));
    PostProcessingEffect brightPassEffect(std::move(brightPassShader));
    brightPassEffect.addUniform(ShaderUniform("brightnessThreshold", 0.3f));
    postProcessing.addEffect("brightPass", std::move(brightPassEffect));

    // DownSample shader
    Shader downSampleShader(FileSystemUtils::getAssetFilePath("shaders/invert.vert"),
        FileSystemUtils::getAssetFilePath("shaders/downsample.frag"));
    PostProcessingEffect downSampleEffect(std::move(downSampleShader));
    downSampleEffect.addUniform(ShaderUniform("resolutionFactor", 0.5f)); // Half-size downsample
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
    upSampleEffect.addUniform(ShaderUniform("textureSize", glm::vec2(1280.0f, 530.0f)));
    upSampleEffect.addUniform(ShaderUniform("inverseTextureSize", glm::vec2(1.0f / 1280.0f, 1.0f / 530.0f)));
    postProcessing.addEffect("upSample", std::move(upSampleEffect));

    // BloomFinal shader
    Shader bloomFinalShader(FileSystemUtils::getAssetFilePath("shaders/invert.vert"),
        FileSystemUtils::getAssetFilePath("shaders/bloom_final.frag"));
    PostProcessingEffect bloomFinalEffect(std::move(bloomFinalShader));
    bloomFinalEffect.addUniform(ShaderUniform("bloomIntensity", 0.65f));
    postProcessing.addEffect("bloomFinal", std::move(bloomFinalEffect));

    activeEffects = {"brightPass", "downSample", "kawaseBlur", "upSample", "bloomFinal" };
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
    glViewport(0, 0, screenWidth, screenHeight); // Reset viewport to full resolution for upsample
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    postProcessing.applyEffect("upSample", currentInputTexture, 0);

    // 5. Apply the bloomFinal effect, combining the original scene and the blurred scene
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Render to the default framebuffer (screen)
    glViewport(0, 0, screenWidth, screenHeight); // Ensure viewport is set for full resolution when rendering to screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    postProcessing.applyEffect("bloomFinal", frameBuffers[0].textureId, frameBuffers[5].textureId);
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