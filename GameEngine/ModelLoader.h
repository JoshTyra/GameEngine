#pragma once

#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "LevelGeometry.h"
#include "TextureLoader.h"
#include "Materials.h"

class ModelLoader {
public:
    // Updated to include optional material list file
    static std::vector<std::unique_ptr<LevelGeometry>> loadModel(const std::string& path, const std::string& materialListFile = "");

private:
    static std::unique_ptr<LevelGeometry> processMesh(aiMesh* mesh, const aiScene* scene, std::shared_ptr<Material> material);
    static std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName, std::vector<Texture>& loadedTextures);

    // New function to read material list
    static std::vector<std::string> readMaterialList(const std::string& materialListFile);
};