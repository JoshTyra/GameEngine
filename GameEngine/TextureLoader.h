#pragma once

#include "Texture.h"
#include <memory>
#include <GL/glew.h>
#include <string>

class TextureLoader {
public:
    static std::unique_ptr<Texture> loadTexture(const std::string& path);
};
