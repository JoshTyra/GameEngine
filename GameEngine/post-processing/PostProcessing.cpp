#include "PostProcessing.h"

PostProcessing::PostProcessing() {
	// Constructor implementation
	// Initialize any members here if necessary
}

PostProcessing::~PostProcessing() {
	// Clean up resources if necessary
}

void PostProcessing::addEffect(const std::string& name, PostProcessingEffect&& effect) {
	effects.emplace(name, std::move(effect));
}

void PostProcessing::applyEffects(GLuint inputTexture) {
	GLuint currentInputTexture = inputTexture;
	bool useFirstFramebuffer = false;

	// Apply each effect in the activeEffects list
	for (const auto& effectName : activeEffects) {
		auto it = effects.find(effectName);
		if (it != effects.end()) {
			// Ping-pong: bind the next framebuffer
			useFirstFramebuffer = !useFirstFramebuffer;
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[useFirstFramebuffer ? 0 : 1]);

			glClearColor(0.0, 0.0, 0.0, 1.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			PostProcessingEffect& effect = it->second;
			effect.shader.use();

			// Set up the texture for the current effect
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, currentInputTexture);
			effect.shader.setInt("screenTexture", 0);

			for (const auto& uniform : effect.uniforms) {
				uniform.applyToShader(effect.shader);
			}

			screenQuad.render();

			// Update currentInputTexture to the one we just rendered to
			currentInputTexture = textures[useFirstFramebuffer ? 0 : 1];
		}
		else {
			std::cerr << "Effect '" << effectName << "' not found in the effects map." << std::endl;
			return;
		}
	}

	// Now render the final texture to the screen
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// The last effect applied will be the current shader in use.
	// Use it to draw the final result to the screen.
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, currentInputTexture); // This is the result of the last effect

	// If your last effect's shader expects the texture to be bound to a different uniform,
	// you will need to set it here
	// effect.shader.setInt("finalTexture", 0); // This line would be needed if the uniform is different

	screenQuad.render();
}

void PostProcessing::setActiveEffects(const std::vector<std::string>& effectNames) {
	activeEffects = effectNames;  // Directly store the names
}

void PostProcessing::initializeFramebuffers(std::vector<std::pair<GLsizei, GLsizei>> resolutions) {
	for (const auto& res : resolutions) {
		GLsizei width = res.first;
		GLsizei height = res.second;
		GLuint framebuffer;
		GLuint texture;

		glGenFramebuffers(1, &framebuffer);
		glGenTextures(1, &texture);

		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			std::cerr << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		}

		framebuffers.push_back(framebuffer);
		textures.push_back(texture);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind to avoid unintended side effects
}


