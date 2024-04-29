#pragma once

#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "StaticGeometry.h"
#include "geometry/AnimatedGeometry.h"
#include "TextureLoader.h"
#include "Materials.h"
#include "rendering/IRenderable.h"
#include "animations/Bone.h"

class ModelLoader {
public:
    static std::tuple<std::vector<std::shared_ptr<StaticGeometry>>, std::vector<std::shared_ptr<AnimatedGeometry>>> loadModel(const std::string& path, const std::string& materialPath);

private:
    static std::shared_ptr<StaticGeometry> processStaticMesh(aiMesh* mesh, const aiScene* scene, std::shared_ptr<Material> material);
    static std::shared_ptr<AnimatedGeometry> processAnimatedMesh(aiMesh* mesh, const aiScene* scene, std::shared_ptr<Material> material);
    static std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName, std::vector<Texture>& loadedTextures);
    static std::vector<std::string> readMaterialList(const std::string& materialListFile);
    static std::vector<std::shared_ptr<Material>> loadMaterials(const std::string& materialPath);

    // Added declarations
    static void SetVertexBoneDataToDefault(AnimatedVertex& vertex);
    static void ExtractBoneWeightForVertices(AnimatedVertex& vertex, aiMesh* mesh, int vertexIndex);
    static void SetVertexBoneData(AnimatedVertex& vertex, int boneID, float weight);

    // Added member variables
    static std::map<std::string, BoneInfo> m_BoneInfoMap;
    static int m_BoneCounter;
};