#include "ModelLoader.h"
#include "FileSystemUtils.h"
#include <Materials.h>
#include <MaterialParser.h>
#include <memory> // For std::shared_ptr
#include <algorithm> // For std::transform
#include <filesystem> // For path operations
#include "Debug.h"

// Helper function to determine file extension
std::string getFileExtension(const std::string& filename) {
    std::filesystem::path path(filename);
    return path.extension().string();
}

std::vector<std::shared_ptr<IRenderable>> ModelLoader::loadModel(const std::string& path, const std::string& materialPath) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        throw std::runtime_error("Failed to load model");
    }

    std::vector<std::shared_ptr<IRenderable>> meshes;
    std::vector<std::shared_ptr<Material>> materials;

    // Check file extension
    std::string extension = getFileExtension(materialPath);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower); // Ensure extension is lowercase

    if (extension == ".txt") {
        // It's a material list file
        auto materialFiles = readMaterialList(materialPath);
        for (const auto& materialFile : materialFiles) {
            materials.push_back(std::make_shared<Material>(MaterialParser::parseMaterialXML(materialFile)));
        }
    }
    else if (extension == ".xml") {
        // It's a single material file
        materials.push_back(std::make_shared<Material>(MaterialParser::parseMaterialXML(materialPath)));
    }

    // Assuming one material per mesh, in order
    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[i];
        std::shared_ptr<Material> material = nullptr;

        if (!materials.empty()) {
            // Use the corresponding material if available, or fallback to the first material
            material = (i < materials.size()) ? materials[i] : materials.front();
        }

        auto geometry = processMesh(mesh, scene, material);
        meshes.push_back(std::move(geometry));
    }

    return meshes;
}

std::shared_ptr<LevelGeometry> ModelLoader::processMesh(aiMesh* mesh, const aiScene* scene, std::shared_ptr<Material> material) {
    // Log number of meshes and current mesh pointer
    DEBUG_COUT << "[Info] Processing mesh " << scene->mNumMeshes << ", pointer: " << mesh << std::endl;

    // Validate mesh pointer
    if (!mesh) {
        std::cerr << "[Error] Skipping mesh due to null pointer." << std::endl;
        return nullptr;
    }

    // Log mesh vertex count
    DEBUG_COUT << "[Info] Mesh vertex count: " << mesh->mNumVertices << std::endl;

    // Initialize data structures
    std::vector<StaticVertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    // Process vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        StaticVertex vertex;

        // Extract and log position
        vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        DEBUG_COUT << "[Info] Vertex " << i << " position: (" << vertex.Position.x << ", " << vertex.Position.y << ", " << vertex.Position.z << ")" << std::endl;

        // Check and handle normals
        if (mesh->HasNormals()) {
            vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
            DEBUG_COUT << "[Info] Vertex " << i << " normal: (" << vertex.Normal.x << ", " << vertex.Normal.y << ", " << vertex.Normal.z << ")" << std::endl;
        }
        else {
            vertex.Normal = glm::vec3(0.0f);
            DEBUG_COUT << "[Info] Vertex " << i << " has no normal, assigning default (0, 0, 0)" << std::endl;
        }

        // Check and handle texture coordinates
        if (mesh->mTextureCoords[0]) {
            vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
            DEBUG_COUT << "[Info] Vertex " << i << " texture coordinates: (" << vertex.TexCoords.x << ", " << vertex.TexCoords.y << ")" << std::endl;
        }
        else {
            vertex.TexCoords = glm::vec2(0.0f);
            DEBUG_COUT << "[Info] Vertex " << i << " has no texture coordinates, assigning default (0, 0)" << std::endl;
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
        DEBUG_COUT << "[Info] Face " << i << ", number of indices: " << face.mNumIndices << std::endl;

        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
            DEBUG_COUT << "[Info] Face " << i << ", index " << j << ": " << face.mIndices[j] << std::endl;
        }
    }

    // Process textures
    for (const auto& [unit, textureName] : material->getTextures()) {
        std::string fullPath = FileSystemUtils::getAssetFilePath("textures/" + textureName);
        DEBUG_COUT << "[Info] Loading texture for unit " << unit << ": " << fullPath.c_str() << std::endl;

        auto texturePtr = TextureLoader::loadTexture(fullPath); // Use the full path
        if (texturePtr) {
            Texture texture;
            texture.id = texturePtr->id; // Access the texture ID
            texture.type = unit;
            texture.path = fullPath; // Store the full path if needed
            textures.push_back(texture);
            DEBUG_COUT << "[Info] Texture loaded successfully for unit " << unit;
        }
        else {
            std::cerr << "Failed to load texture: " << fullPath << std::endl;
        }
    }

    // Process mesh...
    auto geometry = std::make_shared<LevelGeometry>(vertices, indices, textures);
    geometry->setMaterial(material); // Directly use the shared_ptr

    return geometry;
}

std::vector<std::string> ModelLoader::readMaterialList(const std::string& materialListFile) {
    std::vector<std::string> materialPaths;
    std::ifstream file(materialListFile);
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty()) {
            std::string fullPath = FileSystemUtils::getAssetFilePath("materials/" + line);
            materialPaths.push_back(fullPath);
        }
    }
    return materialPaths;
}


