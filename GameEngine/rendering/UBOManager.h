#pragma once
#include <GL/glew.h>
#include <vector>
#include <stdexcept>

class UBOManager {
public:
    UBOManager() {
        GLint maxBindingPoints;
        glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &maxBindingPoints);
        bindingPoints.resize(maxBindingPoints, false);
    }

    // Assigns a free UBO binding point and returns its index
    GLuint assignBindingPoint() {
        for (size_t i = 0; i < bindingPoints.size(); ++i) {
            if (!bindingPoints[i]) {
                bindingPoints[i] = true;
                return static_cast<GLuint>(i);
            }
        }
        throw std::runtime_error("No available UBO binding points.");
    }

    // Releases a UBO binding point for reuse
    void releaseBindingPoint(GLuint point) {
        if (point < bindingPoints.size()) {
            bindingPoints[point] = false;
        }
        else {
            throw std::out_of_range("Trying to release an invalid UBO binding point.");
        }
    }

private:
    std::vector<bool> bindingPoints; // Tracks which binding points are in use
};
