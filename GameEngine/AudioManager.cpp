#include "AudioManager.h"
#include <iostream>

AudioManager::AudioManager() {
    soundEngine = createIrrKlangDevice();
    if (!soundEngine) {
        throw std::runtime_error("Could not create sound engine.");
    }
}

AudioManager::~AudioManager() {
    cleanUp();
}

bool AudioManager::loadSound(const std::string& name, const std::string& filePath) {
    // With irrKlang, we don't preload sounds like in OpenAL. Just store the file path.
    soundPaths[name] = filePath;
    return true;
}

void AudioManager::playSound(const std::string& filePath, const irrklang::vec3df& position, bool loop) {
    if (soundEngine) {
        soundEngine->play3D(filePath.c_str(), position, loop, false, true);
    }
    else {
        std::cerr << "Sound engine not initialized or sound file not found: " << filePath << std::endl;
    }
}

void AudioManager::stopSound(const std::string& name) {
    // irrKlang handles sound stopping internally. We could extend functionality if needed.
}

void AudioManager::updateListenerPosition(const irrklang::vec3df& position, const irrklang::vec3df& lookDirection, const irrklang::vec3df& upVector) {
    soundEngine->setListenerPosition(position, lookDirection, irrklang::vec3df(0, 0, 0), upVector);
}

void AudioManager::cleanUp() {
    if (soundEngine) {
        soundEngine->drop();
    }
}