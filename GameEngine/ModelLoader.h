#pragma once

#include <memory>
#include <algorithm>
#include <filesystem>
#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "StaticGeometry.h"
#include "geometry/AnimatedGeometry.h"
#include "TextureLoader.h"
#include "Materials.h"
#include <MaterialParser.h>
#include "rendering/IRenderable.h"
#include "animations/Bone.h"
#include "FileSystemUtils.h"
#include "Debug.h"
#include <unordered_map>

class ModelLoader {
public:
    static std::vector<std::unique_ptr<IRenderable>> loadModel(const std::string& path, const std::string& materialPath);

private:
    static std::unique_ptr<AnimatedGeometry> processAnimatedMesh(aiMesh* mesh, const aiScene* scene, std::shared_ptr<Material> material);
    static std::unique_ptr<StaticGeometry> processStaticMesh(aiMesh* mesh, const aiScene* scene, std::shared_ptr<Material> material);
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