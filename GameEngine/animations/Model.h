#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <map>
#include "animations/Mesh.h"
#include "animations/Bone.h"
#include "rendering/IRenderable.h"
#include "FileSystemUtils.h"

class Model : public IRenderable {
public:
    Model(const std::string& path, bool gamma = false);
    void Draw(Shader& shader);
    auto& GetBoneInfoMap() { return m_BoneInfoMap; }
    int& GetBoneCount() { return m_BoneCounter; }

private:
    int m_BoneCounter = 0;
    std::vector<Mesh> meshes;
    std::string directory;
    bool gammaCorrection;
    std::map<std::string, BoneInfo> m_BoneInfoMap;
    std::shared_ptr<Material> m_Material;

    void loadModel(const std::string& path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);

    void SetVertexBoneDataToDefault(AnimatedVertex& vertex);
    void SetVertexBoneData(AnimatedVertex& vertex, int boneID, float weight);
    void ExtractBoneWeightForVertices(std::vector<AnimatedVertex>& vertices, aiMesh* mesh, const aiScene* scene);
    // Implement the draw function from the IRenderable interface
    void draw(const RenderingContext& context) override;
    // Implement the setMaterial function from the IRenderable interface
    void setMaterial(std::shared_ptr<Material> material) override;
};