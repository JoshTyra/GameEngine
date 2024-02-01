#pragma once

#include <map>
#include <string>
#include <memory>
#include "Texture.h"
#include "stb_image.h"
#include <imgui_impl_opengl3_loader.h>

class TextureManager {
public:
    static std::shared_ptr<Texture> loadTexture(const std::string& path) {
        auto it = textures.find(path);
        if (it != textures.end()) {
            // Texture already loaded, return it
            return it->second;
        }

        // Load texture using OpenGL and stb_image.h
        unsigned int textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        // ... (OpenGL texture setup and loading using stb_image.h) ...
        glBindTexture(GL_TEXTURE_2D, 0);

        // Create and store the Texture object
        auto texture = std::make_shared<Texture>();
        texture->id = textureID;
        texture->path = path; // Store path for identification
        textures[path] = texture;

        return texture;
    }

    static std::shared_ptr<Texture> getTexture(const std::string& path) {
        auto it = textures.find(path);
        if (it != textures.end()) {
            return it->second;
        }
        return nullptr; // Texture not found
    }

    // ... Additional methods as needed ...

private:
    static std::map<std::string, std::shared_ptr<Texture>> textures;
};
