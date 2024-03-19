#pragma once

#include <glm/glm.hpp>

struct StaticVertex {
    glm::vec3 Position{ 0.0f, 0.0f, 0.0f };
    glm::vec3 Normal{ 0.0f, 0.0f, 0.0f };
    glm::vec2 TexCoords{ 0.0f, 0.0f };
    glm::vec2 LightMapTexCoords{ 0.0f, 0.0f };
};
