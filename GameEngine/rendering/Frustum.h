// Frustum.h
#pragma once
#include <glm/glm.hpp>

struct Plane {
    glm::vec3 normal;
    float distance;

    void normalize() {
        float mag = glm::length(normal);
        normal = normal / mag;
        distance = distance / mag;
    }
};

struct Frustum {
    Plane planes[6];

    void update(const glm::mat4& VPMatrix);
};

