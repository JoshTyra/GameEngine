#pragma once

#include "SkinnedMesh.h"
#include "geometry/Animation.h"
#include "rendering/IRenderable.h"
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <assimp/scene.h> // Ensure you have this include for aiScene
#include <glm/glm.hpp>
#include <assimp/types.h> // Make sure to include the Assimp header that defines aiVector3D
#include <glm/gtc/quaternion.hpp>
#include <assimp/quaternion.h>

class AnimatedModel : public IRenderable {
public:
    Shader shader;
    AnimatedModel(const std::string& vertexPath, const std::string& fragmentPath);
    ~AnimatedModel();

    bool loadModel(const std::string& path);
    void update(float deltaTime);
    void draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) const override;
    void setAnimation(const std::string& animationName);

private:
    void loadAnimations(const aiScene* scene); // Make sure to declare this method

    std::vector<SkinnedMesh> meshes;
    std::vector<std::shared_ptr<SkinnedMesh>> skinnedMeshes;
    std::shared_ptr<Skeleton> skeleton; // Use smart pointers for shared resources
    std::map<std::string, std::shared_ptr<Animation>> animations; // Allows dynamic animation swapping
    std::shared_ptr<Animation> currentAnimation; // Optional: Direct reference to the current animation
};
