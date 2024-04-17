#pragma once

#include "Texture.h"
#include <memory>
#include <GL/glew.h>
#include <string>
#include <vector>
#include <map>

class TextureLoader {
public:
    static Texture loadTexture(const std::string& path);
    void static loadCubemapFace(GLuint cubemapTextureId, const std::string& path, GLenum face);
    static Texture createCubemap(const std::vector<std::string>& paths);
    static GLenum getGLCubemapFace(const std::string& faceName);  // Helper function
private:
    static GLenum mapFaceNameToGLenum(const std::string& faceName);
};
