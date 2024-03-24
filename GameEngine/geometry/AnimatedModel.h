#pragma once

#include "SkinnedMesh.h"
#include "geometry/Animation.h"
#include "rendering/IRenderable.h"
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <assimp/types.h>
#include <glm/gtc/quaternion.hpp>
#include <assimp/quaternion.h>
#include "utilities/MathUtils.h"

#define MAX_BONES 100

class AnimatedModel : public IRenderable {
public:
    Shader shader;
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;

    AnimatedModel(const std::string& vertexPath, const std::string& fragmentPath);
    ~AnimatedModel();

    bool loadModel(const std::string& path);
    void update(float deltaTime);
    void draw(const RenderingContext& context) const override;
    void setAnimation(const std::string& animationName);
    void createBoneMatricesBuffer();
    void updateShaderWithBoneMatrices();
    void updateBoneMatrices(const std::vector<glm::mat4>& boneMatrices);
    void processBonesAndHierarchy(const aiScene* scene, std::shared_ptr<Skeleton> skeleton);
    void establishHierarchy(const aiNode* node, std::shared_ptr<Bone> parentBone, std::shared_ptr<Skeleton> skeleton, std::unordered_map<std::string, std::shared_ptr<Bone>>& tempBoneMap);
    glm::mat4 getModelMatrix() const;

private:
    void loadAnimations(const aiScene* scene);
    std::vector<std::shared_ptr<SkinnedMesh>> skinnedMeshes;
    std::shared_ptr<Skeleton> skeleton;
    std::map<std::string, std::shared_ptr<Animation>> animations;
    std::shared_ptr<Animation> currentAnimation;
    float currentAnimationTime = 0.0f;
    GLuint uboBoneMatrices;
};