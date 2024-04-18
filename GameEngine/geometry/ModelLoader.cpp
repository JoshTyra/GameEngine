#include "ModelLoader.h"
#include "FileSystemUtils.h"
#include <Materials.h>
#include <MaterialParser.h>
#include <memory> // For std::shared_ptr
#include <algorithm> // For std::transform
#include <filesystem> // For path operations
#include "Debug.h"

std::map<std::string, BoneInfo> ModelLoader::m_BoneInfoMap;
int ModelLoader::m_BoneCounter = 0;

// Helper function to determine file extension
std::string getFileExtension(const std::string& filename) {
    std::filesystem::path path(filename);
    return path.extension().string();
}

std::tuple<std::vector<std::shared_ptr<StaticGeometry>>, std::vector<std::shared_ptr<AnimatedGeometry>>> ModelLoader::loadModel(const std::string& path, const std::string& materialPath) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_OptimizeGraph | aiProcess_OptimizeMeshes);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        throw std::runtime_error("Failed to load model");
    }

    std::vector<std::shared_ptr<StaticGeometry>> staticMeshes;
    std::vector<std::shared_ptr<AnimatedGeometry>> animatedMeshes;

    std::vector<std::shared_ptr<Material>> materials = loadMaterials(materialPath);

    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[i];
        std::shared_ptr<Material> material = nullptr;

        if (!materials.empty()) {
            // Use the corresponding material if available, or fallback to the first material
            material = (i < materials.size()) ? materials[i] : materials.front();
        }

        if (mesh->HasBones()) {
            auto animatedGeometry = processAnimatedMesh(mesh, scene, material);
            animatedMeshes.push_back(std::move(animatedGeometry));
        }
        else {
            auto staticGeometry = processStaticMesh(mesh, scene, material);
            staticMeshes.push_back(std::move(staticGeometry));
        }
    }

    return std::make_tuple(staticMeshes, animatedMeshes);
}

std::vector<std::shared_ptr<Material>> ModelLoader::loadMaterials(const std::string& materialPath) {
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

    return materials;
}

std::shared_ptr<StaticGeometry> ModelLoader::processStaticMesh(aiMesh* mesh, const aiScene* scene, std::shared_ptr<Material> material) {
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
        if (unit == "environment") {
            // Retrieve cubemap faces from the material
            auto cubemapFaces = material->getCubemapFaces(); // Function to retrieve map of faces
            std::vector<std::string> paths;

            //std::cout << "Cubemap faces order from processStaticMesh:" << std::endl;
            for (const auto& [faceName, path] : cubemapFaces) {
                std::string fullPath = FileSystemUtils::getAssetFilePath("textures/" + path);
                paths.push_back(fullPath); // Collect all paths for the cubemap

                //std::cout << "Face: " << faceName << ", Path: " << fullPath << std::endl;
            }

            // Use the new method to get or create a cubemap from the paths
            Texture cubemapTexture = TextureLoader::createCubemap(paths);
            if (cubemapTexture.id != 0) { // Check if texture was successfully created or retrieved from cache
                cubemapTexture.type = "environment";
                textures.push_back(cubemapTexture); // Push back the created or cached cubemap texture to the geometry's textures list
                //std::cout << "Cubemap texture added to textures vector. Type: " << cubemapTexture.type << ", ID: " << cubemapTexture.id << std::endl;
            }
            else {
                std::cerr << "Failed to load or retrieve cubemap texture." << std::endl;
            }
        }
        else {
            // Load other textures normally
            std::string fullPath = FileSystemUtils::getAssetFilePath("textures/" + textureName);
            Texture texture = TextureLoader::loadTexture(fullPath);
            if (texture.id != 0) { // Assuming 0 is used to denote failure to load
                texture.type = unit;
                texture.path = fullPath;
                textures.push_back(texture);
            }
        }
    }

    // Process mesh...
    auto geometry = std::make_shared<StaticGeometry>(vertices, indices, textures);
    geometry->setMaterial(material); // Directly use the shared_ptr

    return geometry;
}

std::shared_ptr<AnimatedGeometry> ModelLoader::processAnimatedMesh(aiMesh* mesh, const aiScene* scene, std::shared_ptr<Material> material) {
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
    std::vector<AnimatedVertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    // Process vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        AnimatedVertex vertex;

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

        // Extract bone IDs and weights
        SetVertexBoneDataToDefault(vertex);
        ExtractBoneWeightForVertices(vertex, mesh, i);

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
        if (unit == "environment") {
            // Retrieve cubemap faces from the material
            auto cubemapFaces = material->getCubemapFaces(); // Function to retrieve map of faces
            std::vector<std::string> paths;
            for (const auto& [faceName, path] : cubemapFaces) {
                std::string fullPath = FileSystemUtils::getAssetFilePath("textures/" + path);
                paths.push_back(fullPath); // Collect all paths for the cubemap
            }

            // Use the new method to get or create a cubemap from the paths
            Texture cubemapTexture = TextureLoader::createCubemap(paths);
            if (cubemapTexture.id != 0) { // Check if texture was successfully created or retrieved from cache
                cubemapTexture.type = "environment";
                textures.push_back(cubemapTexture); // Push back the created or cached cubemap texture to the geometry's textures list
                std::cout << "Cubemap texture added to textures vector. Type: " << cubemapTexture.type << ", ID: " << cubemapTexture.id << std::endl;
            }
            else {
                std::cerr << "Failed to load or retrieve cubemap texture." << std::endl;
            }
        }
        else {
            // Load other textures normally
            std::string fullPath = FileSystemUtils::getAssetFilePath("textures/" + textureName);
            Texture texture = TextureLoader::loadTexture(fullPath);
            if (texture.id != 0) { // Assuming 0 is used to denote failure to load
                texture.type = unit;
                texture.path = fullPath;
                textures.push_back(texture);
            }
        }
    }

    auto geometry = std::make_shared<AnimatedGeometry>(vertices, indices, textures, m_BoneInfoMap);
    geometry->setMaterial(material);

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

void ModelLoader::SetVertexBoneDataToDefault(AnimatedVertex& vertex) {
    for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
        vertex.m_BoneIDs[i] = -1;
        vertex.m_Weights[i] = 0.0f;
    }
}

void ModelLoader::ExtractBoneWeightForVertices(AnimatedVertex& vertex, aiMesh* mesh, int vertexIndex) {
    for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
        std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
        int boneID;
        if (m_BoneInfoMap.find(boneName) == m_BoneInfoMap.end()) {
            BoneInfo newBoneInfo;
            newBoneInfo.id = m_BoneCounter;
            newBoneInfo.offset = AssimpGLMHelpers::ConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);
            m_BoneInfoMap[boneName] = newBoneInfo;
            boneID = m_BoneCounter++;
        }
        else {
            boneID = m_BoneInfoMap[boneName].id;
        }

        auto weights = mesh->mBones[boneIndex]->mWeights;
        for (int weightIndex = 0; weightIndex < mesh->mBones[boneIndex]->mNumWeights; ++weightIndex) {
            if (weights[weightIndex].mVertexId == vertexIndex) {
                float weight = weights[weightIndex].mWeight;
                SetVertexBoneData(vertex, boneID, weight);
                break;
            }
        }
    }
}

void ModelLoader::SetVertexBoneData(AnimatedVertex& vertex, int boneID, float weight) {
    for (int i = 0; i < MAX_BONE_INFLUENCE; ++i) {
        if (vertex.m_BoneIDs[i] < 0) {
            vertex.m_Weights[i] = weight;
            vertex.m_BoneIDs[i] = boneID;
            break;
        }
    }
}


