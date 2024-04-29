#include "TextureLoader.h"

std::map<std::string, GLuint> TextureLoader::cubemapCache = {};

// Load a single 2D texture
Texture TextureLoader::loadTexture(const std::string& path) {
    Texture texture;
    glGenTextures(1, &texture.id);
    glBindTexture(GL_TEXTURE_2D, texture.id);
    int width, height, nrChannels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
    }
    else {
        std::cerr << "Texture failed to load at path: " << path << std::endl;
        return {}; // Return empty texture (check if texture.id == 0 in client code)
    }
    texture.path = path;
    return texture;
}

Texture TextureLoader::createCubemap(const std::vector<std::string>& paths) {
    if (paths.size() != 6) {
        std::cerr << "Cubemap must have exactly six faces" << std::endl;
        return {}; // Return an empty texture
    }

    // Create a unique key for the cubemap cache based on the paths
    std::ostringstream oss;
    for (const auto& path : paths) {
        oss << path << ";";
    }
    std::string key = oss.str();

    // Check if the cubemap is already loaded
    auto it = cubemapCache.find(key);
    if (it != cubemapCache.end()) {
        // Found in cache, return existing cubemap texture
        Texture cachedTexture;
        cachedTexture.id = it->second;
        cachedTexture.type = "environment";
        return cachedTexture;
    }

    Texture cubemapTexture;
    glGenTextures(1, &cubemapTexture.id);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture.id);

    for (int i = 0; i < 6; i++) {
        GLenum face = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;
        int width, height, nrChannels;
        unsigned char* data = stbi_load(paths[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
            glTexImage2D(face, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else {
            std::cerr << "Failed to load cubemap face: " << paths[i] << std::endl;
        }
    }

    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    return cubemapTexture;
}

GLenum TextureLoader::getGLCubemapFace(const std::string& faceName) {
    static const std::map<std::string, GLenum> faceMap = {
        {"right", GL_TEXTURE_CUBE_MAP_POSITIVE_X},
        {"left", GL_TEXTURE_CUBE_MAP_NEGATIVE_X},
        {"top", GL_TEXTURE_CUBE_MAP_POSITIVE_Y},
        {"bottom", GL_TEXTURE_CUBE_MAP_NEGATIVE_Y},
        {"front", GL_TEXTURE_CUBE_MAP_POSITIVE_Z},
        {"back", GL_TEXTURE_CUBE_MAP_NEGATIVE_Z}
    };
    auto it = faceMap.find(faceName);
    if (it != faceMap.end()) {
        return it->second;
    }
    std::cerr << "Invalid cubemap face name: " << faceName << std::endl;
    return 0; // Return an invalid value or handle this error appropriately
}


