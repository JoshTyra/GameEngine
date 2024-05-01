// LODManager.cpp
#include "LODManager.h"
#include "StaticGeometry.h"
#include "geometry/AnimatedGeometry.h"

LODManager::LODManager(const std::vector<float>& distanceThresholds)
    : m_distanceThresholds(distanceThresholds) {
}

int LODManager::getLODLevel(const glm::vec3& cameraPosition, const StaticGeometry& geometry) const {
    // Calculate the distance between the camera and the geometry's bounding box center
    glm::vec3 boundingBoxCenter = (geometry.getAABBMin() + geometry.getAABBMax()) * 0.5f;
    float distance = glm::distance(cameraPosition, boundingBoxCenter);

    return calculateLODLevel(distance);
}

int LODManager::getLODLevel(const glm::vec3& cameraPosition, const AnimatedGeometry& geometry) const {
    // Calculate the distance between the camera and the geometry's bounding box center
    glm::vec3 boundingBoxCenter = (geometry.getAABBMin() + geometry.getAABBMax()) * 0.5f;
    float distance = glm::distance(cameraPosition, boundingBoxCenter);

    return calculateLODLevel(distance);
}

int LODManager::calculateLODLevel(float distance) const {
    int lodLevel = 0;
    for (size_t i = 0; i < m_distanceThresholds.size(); ++i) {
        if (distance < m_distanceThresholds[i]) {
            break;
        }
        lodLevel = i + 1;
    }
    return lodLevel;
}