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

void AudioManager::playSound(const std::string& name, const glm::vec3& position, bool loop, float rolloffFactor, float referenceDistance, float maxDistance) {
    auto it = soundBuffers.find(name);
    if (it == soundBuffers.end()) {
        std::cerr << "Sound not found: " << name << std::endl;
        return;
    }

    ALuint source;
    alGenSources(1, &source); // Generate a new source
    alSourcei(source, AL_BUFFER, it->second); // Attach the buffer to the source
    alSourcei(source, AL_LOOPING, loop ? AL_TRUE : AL_FALSE); // Set looping
    alSource3f(source, AL_POSITION, position.x, position.y, position.z); // Set the source's position

    // Update or add the source information
    activeSources[name] = { source, position };

    // Debug output for sound source position
    std::cout << "Sound Source Position: X=" << position.x << " Y=" << position.y << " Z=" << position.z << std::endl;

    // Set attenuation properties
    alSourcef(source, AL_ROLLOFF_FACTOR, rolloffFactor);
    alSourcef(source, AL_REFERENCE_DISTANCE, referenceDistance);
    alSourcef(source, AL_MAX_DISTANCE, maxDistance);

    // Print detailed info about the sound being played
    std::cout << "Playing sound: " << name << std::endl;
    std::cout << "Looping: " << (loop ? "Yes" : "No") << std::endl;
    std::cout << "Rolloff Factor: " << rolloffFactor << std::endl;
    std::cout << "Reference Distance: " << referenceDistance << std::endl;
    std::cout << "Max Distance: " << maxDistance << std::endl;

    alSourcePlay(source); // Play the source
}

void AudioManager::cleanupSources() {
    auto it = activeSources.begin();
    while (it != activeSources.end()) {
        ALint state;
        alGetSourcei(it->second.sourceID, AL_SOURCE_STATE, &state);
        if (state != AL_PLAYING && state != AL_PAUSED) {
            alDeleteSources(1, &it->second.sourceID); // Delete the source
            it = activeSources.erase(it); // Remove from the map and get the next iterator
        }
        else {
            ++it;
        }
    }
}

void AudioManager::cleanUp() {
    // Delete any active sources to ensure no sources are left playing or allocated
    for (auto& pair : activeSources) {
        alDeleteSources(1, &pair.second.sourceID);
    }
    activeSources.clear(); // Clear the map of active sources

    // Delete the buffers as before
    for (auto& entry : soundBuffers) {
        alDeleteBuffers(1, &entry.second);
    }
    soundBuffers.clear();

    // Clean up the OpenAL context and device as before
    if (context) {
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(context);
        context = nullptr; // Set to nullptr to avoid dangling pointers
    }
    if (device) {
        alcCloseDevice(device);
        device = nullptr; // Set to nullptr for safety
    }
}

void AudioManager::updateSourcePositions() {
    for (auto& pair : activeSources) {
        const SourceInfo& info = pair.second;
        std::cout << "Updated source position for: " << pair.first << " to X=" << info.position.x << " Y=" << info.position.y << " Z=" << info.position.z << std::endl;
        alSource3f(info.sourceID, AL_POSITION, info.position.x, info.position.y, info.position.z);
    }
}
