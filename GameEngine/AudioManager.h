#pragma once

#include <irrKlang.h>
#include <string>
#include <memory>
#include <unordered_map>
#include <glm/ext/vector_float3.hpp>

using namespace irrklang;

// Simplified sound info structure for irrKlang
struct SoundInfo {
    ISound* sound;
    glm::vec3 position;
};

class AudioManager {
public:
    AudioManager();
    ~AudioManager();

    bool loadSound(const std::string& name, const std::string& filePath);
    void playSound(const std::string& filePath, const irrklang::vec3df& position, bool loop);
    void stopSound(const std::string& name);
    void updateListenerPosition(const irrklang::vec3df& position, const irrklang::vec3df& lookDirection, const irrklang::vec3df& upVector);
    void cleanUp();

private:
    ISoundEngine* soundEngine;
    std::unordered_map<std::string, std::string> soundPaths; // Maps sound names to file paths
};