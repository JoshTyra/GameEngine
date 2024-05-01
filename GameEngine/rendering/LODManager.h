// LODManager.h
#pragma once

#include <vector>
#include <glm/glm.hpp>

class StaticGeometry;
class AnimatedGeometry;

class LODManager {
public:
    LODManager(const std::vector<float>& distanceThresholds);

    int getLODLevel(const glm::vec3& cameraPosition, const StaticGeometry& geometry) const;
    int getLODLevel(const glm::vec3& cameraPosition, const AnimatedGeometry& geometry) const;

private:
    std::vector<float> m_distanceThresholds;

    int calculateLODLevel(float distance) const;
};