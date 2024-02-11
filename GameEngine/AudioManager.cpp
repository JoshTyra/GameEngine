#include "AudioManager.h"
#include <iostream>
#include "stb_vorbis.h"

AudioManager::AudioManager() : device(nullptr), context(nullptr) {
    device = alcOpenDevice(nullptr);
    if (!device) {
        throw std::runtime_error("Failed to open the default OpenAL device.");
    }

    context = alcCreateContext(device, nullptr);
    if (!context || !alcMakeContextCurrent(context)) {
        throw std::runtime_error("Failed to create and set an OpenAL context.");
    }
}

AudioManager::~AudioManager() {
    cleanUp();
}

bool AudioManager::loadSound(const std::string& name, const std::string& filePath) {
    int channels, sampleRate;
    short* decodedData;
    int sampleCount = stb_vorbis_decode_filename(filePath.c_str(), &channels, &sampleRate, &decodedData);

    if (decodedData == NULL) {
        std::cerr << "Failed to load and decode the OGG file: " << filePath << std::endl;
        return false;
    }

    ALenum format;
    if (channels == 1)
        format = AL_FORMAT_MONO16;
    else if (channels == 2)
        format = AL_FORMAT_STEREO16;
    else {
        // Unsupported number of channels
        free(decodedData);
        return false;
    }

    // Generate a buffer and store sound data
    ALuint buffer;
    alGenBuffers(1, &buffer);
    alBufferData(buffer, format, decodedData, sampleCount * sizeof(short) * channels, sampleRate);

    // Insert the buffer into the map
    soundBuffers[name] = buffer;

    // Free the memory allocated by stb_vorbis_decode_filename
    free(decodedData);

    return true;
}

void AudioManager::playSound(const std::string& name, const glm::vec3& position, bool loop) {
    auto it = soundBuffers.find(name);
    if (it == soundBuffers.end()) {
        std::cerr << "Sound not found: " << name << std::endl;
        return;
    }

    ALuint source;
    alGenSources(1, &source);
    alSourcei(source, AL_BUFFER, it->second);
    alSourcei(source, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
    alSource3f(source, AL_POSITION, position.x, position.y, position.z);
    alSourcePlay(source);

    // Note: In a complete implementation, you might want to keep track of sources to manage their lifecycle
}

void AudioManager::cleanUp() {
    for (auto& entry : soundBuffers) {
        alDeleteBuffers(1, &entry.second);
    }
    soundBuffers.clear();

    if (context) {
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(context);
    }
    if (device) {
        alcCloseDevice(device);
    }
}