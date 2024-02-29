#pragma once
#include <GL/glew.h>

class ScreenQuad {
public:
    ScreenQuad();
    ~ScreenQuad();
    void render() const;

private:
    GLuint vao, vbo;
    void setupQuad();
};
