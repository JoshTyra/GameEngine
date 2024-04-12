#pragma once

#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "StaticGeometry.h"
#include "TextureLoader.h"
#include "Materials.h"
#include "rendering/IRenderable.h"

class ModelLoader {
public:
    static std::vector<std::shared_ptr<IRenderable>> loadModel(const std::string& path, const std::string& materialListFile = "");

private:
    static std::shared_ptr<StaticGeometry> processMesh(aiMesh* mesh, const aiScene* scene, std::shared_ptr<Material> material);
    static std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName, std::vector<Texture>& loadedTextures);
    static std::vector<std::string> readMaterialList(const std::string& materialListFile);
};