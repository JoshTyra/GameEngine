#ifndef OPENGL_UTILS_H
#define OPENGL_UTILS_H

// GLEW
#define GLEW_STATIC // Use this if you've linked GLEW statically
#include <GL/glew.h>

#include <string>

void CheckGLErrors(const std::string& label);

#endif // OPENGL_UTILS_H
