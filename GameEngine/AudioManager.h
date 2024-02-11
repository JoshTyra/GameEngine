#pragma once

#include <al.h>
#include <alc.h>
#include <string>
#include <memory>
#include <unordered_map>
#include <glm/ext/vector_float3.hpp>

class AudioManager {
public:
    AudioManager();
    ~AudioManager();

    bool loadSound(const std::string& name, const std::string& filePath);
    void playSound(const std::string& name, const glm::vec3& position, bool loop = false);

private:
    ALCdevice* device;
    ALCcontext* context;
    std::unordered_map<std::string, ALuint> soundBuffers;

    void cleanUp();
};
