#include "ModelLoader.h"
#include "FileSystemUtils.h"
#include <Materials.h>
#include <MaterialParser.h>
#include <memory> // For std::shared_ptr

std::vector<std::unique_ptr<LevelGeometry>> ModelLoader::loadModel(const std::string& path, const std::string& materialListFile) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        throw std::runtime_error("Failed to load model");
    }

    std::vector<std::unique_ptr<LevelGeometry>> meshes;

    std::shared_ptr<Material> sharedMaterial;
    if (!materialListFile.empty()) {
        sharedMaterial = std::make_shared<Material>(MaterialParser::parseMaterialXML(materialListFile));
    }

    // Process each mesh in the scene
    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[i];
        auto geometry = processMesh(mesh, scene, sharedMaterial); // Adjust processMesh to accept shared_ptr
        meshes.push_back(std::move(geometry));
    }

    return meshes;
}

std::unique_ptr<LevelGeometry> ModelLoader::processMesh(aiMesh* mesh, const aiScene* scene, std::shared_ptr<Material> material) {
    // Log number of meshes and current mesh pointer
    std::cout << "[Info] Processing mesh " << scene->mNumMeshes << ", pointer: " << mesh << std::endl;

    // Validate mesh pointer
    if (!mesh) {
        std::cerr << "[Error] Skipping mesh due to null pointer." << std::endl;
        return nullptr;
    }

    // Log mesh vertex count
    std::cout << "[Info] Mesh vertex count: " << mesh->mNumVertices << std::endl;

    // Initialize data structures
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    // Process vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;

        // Extract and log position
        vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        std::cout << "[Info] Vertex " << i << " position: (" << vertex.Position.x << ", " << vertex.Position.y << ", " << vertex.Position.z << ")" << std::endl;

        // Check and handle normals
        if (mesh->HasNormals()) {
            vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
            std::cout << "[Info] Vertex " << i << " normal: (" << vertex.Normal.x << ", " << vertex.Normal.y << ", " << vertex.Normal.z << ")" << std::endl;
        }
        else {
            vertex.Normal = glm::vec3(0.0f);
            std::cout << "[Info] Vertex " << i << " has no normal, assigning default (0, 0, 0)" << std::endl;
        }

        // Check and handle texture coordinates
        if (mesh->mTextureCoords[0]) {
            vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
            std::cout << "[Info] Vertex " << i << " texture coordinates: (" << vertex.TexCoords.x << ", " << vertex.TexCoords.y << ")" << std::endl;
        }
        else {
            vertex.TexCoords = glm::vec2(0.0f);
            std::cout << "[Info] Vertex " << i << " has no texture coordinates, assigning default (0, 0)" << std::endl;
        }

        if (mesh->HasTextureCoords(1)) { // Check for the existence of the second UV set
            vertex.LightMapTexCoords = glm::vec2(mesh->mTextureCoords[1][i].x, mesh->mTextureCoords[1][i].y);
        }
        else {
            vertex.LightMapTexCoords = glm::vec2(0.0f); // or some default value
        }

        vertices.push_back(vertex);
    }

    // Process indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        // Log face index and number of indices
        std::cout << "[Info] Face " << i << ", number of indices: " << face.mNumIndices << std::endl;

        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
            std::cout << "[Info] Face " << i << ", index " << j << ": " << face.mIndices[j] << std::endl;
        }
    }

    // Process textures
    for (const auto& [unit, textureName] : material->getTextures()) {
        std::string fullPath = FileSystemUtils::getAssetFilePath("textures/" + textureName);
        std::cout << "[Info] Loading texture for unit " << unit << ": " << fullPath.c_str() << std::endl;

        auto texturePtr = TextureLoader::loadTexture(fullPath); // Use the full path
        if (texturePtr) {
            Texture texture;
            texture.id = texturePtr->id; // Access the texture ID
            texture.type = unit;
            texture.path = fullPath; // Store the full path if needed
            textures.push_back(texture);
            std::cout << "[Info] Texture loaded successfully for unit " << unit;
        }
        else {
            std::cerr << "Failed to load texture: " << fullPath << std::endl;
        }
    }

    // Process mesh...
    auto geometry = std::make_unique<LevelGeometry>(vertices, indices, textures);
    geometry->setMaterial(material); // Directly use the shared_ptr

    return geometry;
}

