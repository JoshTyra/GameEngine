#include "ModelLoader.h"
#include "FileSystemUtils.h"
#include <Materials.h>
#include <MaterialParser.h>

std::vector<std::unique_ptr<LevelGeometry>> ModelLoader::loadModel(const std::string& path, const std::string& materialListFile) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        throw std::runtime_error("Failed to load model");
    }

    std::vector<std::unique_ptr<LevelGeometry>> meshes;

    // Load and parse the material file if provided
    Material material;
    if (!materialListFile.empty()) {
        material = MaterialParser::parseMaterialXML(materialListFile);
    }

    // Process each mesh in the scene
    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[i];
        auto geometry = processMesh(mesh, scene, material); // Pass the material to processMesh
        meshes.push_back(std::move(geometry));
    }

    return meshes;
}

std::unique_ptr<LevelGeometry> ModelLoader::processMesh(aiMesh* mesh, const aiScene* scene, const Material& material) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    // Process vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        // Positions
        vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        // Normals
        vertex.Normal = mesh->HasNormals() ? glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z) : glm::vec3(0.0f);
        // Texture Coordinates
        if (mesh->mTextureCoords[0]) {
            vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        }
        else {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }
        vertices.push_back(vertex);
    }

    // Process indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    for (const auto& [unit, textureName] : material.getTextures()) {
        auto texturePtr = TextureLoader::loadTexture(textureName); // This returns std::unique_ptr<Texture>
        if (texturePtr) {
            Texture texture;
            texture.id = texturePtr->id; // Correctly access the id
            texture.type = unit;
            texture.path = textureName;
            textures.push_back(texture);
        }
        else {
            // Handle the case where the texture failed to load
            std::cerr << "Failed to load texture: " << textureName << std::endl;
        }
    }

    // Create LevelGeometry object with vertices, indices, and textures
    auto geometry = std::make_unique<LevelGeometry>(vertices, indices);
    for (const auto& texture : textures) {
        geometry->addTexture(texture); // Ensure this method exists and correctly associates textures with the geometry
    }

    return geometry;
}

