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

void AudioManager::playSound(const std::string& name, const std::string& filePath, const irrklang::vec3df& position, bool loop, float minDistance) {
    if (!soundEngine) {
        std::cerr << "Sound engine not initialized." << std::endl;
        return;
    }

    // Check if the sound source is already loaded
    auto it = soundSources.find(name);
    if (it == soundSources.end()) {
        // Load the sound source if it hasn't been loaded yet
        ISoundSource* source = soundEngine->addSoundSourceFromFile(filePath.c_str());
        if (source) {
            source->setDefaultMinDistance(minDistance);
            soundSources[name] = source;
        }
        else {
            std::cerr << "Failed to load sound file: " << filePath << std::endl;
            return;
        }
    }

    // Play the sound with the specified properties
    soundEngine->play3D(soundSources[name], position, loop, false, true);
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