#pragma once

#include "Texture.h"
#include <memory>
#include <GL/glew.h>
#include <string>
#include <vector>
#include <map>
#include "stb_image.h"
#include <iostream>
#include <memory>
#include <sstream>

class TextureLoader {
public:
    static Texture loadTexture(const std::string& path);
    static Texture createCubemap(const std::vector<std::string>& paths);
    static GLenum getGLCubemapFace(const std::string& faceName);  // Helper function
private:
    static GLenum mapFaceNameToGLenum(const std::string& faceName);
    static std::map<std::string, GLuint> cubemapCache;
};
