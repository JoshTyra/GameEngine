#include "OpenGLUtils.h"
#include <GL/gl.h>
#include <iostream>

void CheckGLErrors(const std::string& label) {
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL Error at " << label << ": " << err << std::endl;
    }
}
