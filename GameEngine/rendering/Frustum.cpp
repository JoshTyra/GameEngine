// Frustum.cpp
#include "Frustum.h"

void Frustum::update(const glm::mat4& VPMatrix) {
    glm::mat4 transposed = glm::transpose(VPMatrix);

    planes[0].normal = glm::vec3(transposed[3] + transposed[0]);
    planes[1].normal = glm::vec3(transposed[3] - transposed[0]);
    planes[2].normal = glm::vec3(transposed[3] + transposed[1]);
    planes[3].normal = glm::vec3(transposed[3] - transposed[1]);
    planes[4].normal = glm::vec3(transposed[3] + transposed[2]);
    planes[5].normal = glm::vec3(transposed[3] - transposed[2]);

    planes[0].distance = transposed[3].w + transposed[0].w;
    planes[1].distance = transposed[3].w - transposed[0].w;
    planes[2].distance = transposed[3].w + transposed[1].w;
    planes[3].distance = transposed[3].w - transposed[1].w;
    planes[4].distance = transposed[3].w + transposed[2].w;
    planes[5].distance = transposed[3].w - transposed[2].w;

    for (int i = 0; i < 6; ++i) {
        planes[i].normalize();
    }
}

