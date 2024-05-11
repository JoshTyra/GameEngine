// Frustum.cpp
#include "Frustum.h"

Frustum createFrustumFromCamera(const CameraNode& cam, float aspect, float fov, float zNear, float zFar)
{
    Frustum frustum;
    const float halfVSide = zFar * tanf(fov * .5f);
    const float halfHSide = halfVSide * aspect;
    const glm::vec3 frontMultFar = zFar * cam.getCameraFront();

    frustum.nearFace = { cam.getCameraPosition() + zNear * cam.getCameraFront(), cam.getCameraFront()};
    frustum.farFace = { cam.getPosition() + frontMultFar, -cam.getCameraFront()};
    frustum.rightFace = { cam.getPosition(), glm::cross(frontMultFar - cam.getCameraRight() * halfHSide, cam.getCameraUp())};
    frustum.leftFace = { cam.getCameraPosition(), glm::cross(cam.getCameraUp(), frontMultFar + cam.getCameraRight() * halfHSide)};
    frustum.topFace = { cam.getCameraPosition(), glm::cross(cam.getCameraRight(), frontMultFar - cam.getCameraUp() * halfVSide)};
    frustum.bottomFace = { cam.getCameraPosition(), glm::cross(frontMultFar + cam.getCameraUp() * halfVSide, cam.getCameraRight())};
    return frustum;
}

