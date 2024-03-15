#pragma once

#include "SkinnedMesh.h"
#include <string>
#include <vector>

class AnimatedModel {
public:
    AnimatedModel();
    ~AnimatedModel();

    // Load model with animations
    bool loadModel(const std::string& path);

    // Update the model's state for animation
    void update(float deltaTime);

    // Render the model
    void render();

private:
    std::vector<SkinnedMesh> meshes;
    // Add skeleton and animation data structures here
};
