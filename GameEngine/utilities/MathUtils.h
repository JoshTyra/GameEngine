// MathUtils.h

#pragma once

#include <glm/glm.hpp>
#include <assimp/matrix4x4.h> // Ensure this is the correct path

namespace MathUtils {
    inline glm::mat4 convertMatrixToGLMFormat(const aiMatrix4x4& from) {
        glm::mat4 to;
        // Copy the data from 'from' to 'to', taking care to transpose or rearrange as necessary
        to[0][0] = from.a1; to[1][0] = from.b1; to[2][0] = from.c1; to[3][0] = from.d1;
        to[0][1] = from.a2; to[1][1] = from.b2; to[2][1] = from.c2; to[3][1] = from.d2;
        to[0][2] = from.a3; to[1][2] = from.b3; to[2][2] = from.c3; to[3][2] = from.d3;
        to[0][3] = from.a4; to[1][3] = from.b4; to[2][3] = from.c4; to[3][3] = from.d4;
        return to;
    }
}
