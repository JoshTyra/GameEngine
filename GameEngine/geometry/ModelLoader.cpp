#include "ModelLoader.h"

std::unordered_map<std::string, std::vector<std::shared_ptr<Material>>> materialCache; // Global material cache
std::map<std::string, BoneInfo> ModelLoader::m_BoneInfoMap;
int ModelLoader::m_BoneCounter = 0;
Assimp::Importer ModelLoader::importer;

// Helper function to determine file extension
std::string getFileExtension(const std::string& filename) {
    std::filesystem::path path(filename);
    return path.extension().string();
}

void NormalizeVertexWeights(AnimatedVertex& vertex) {
    float totalWeight = vertex.Weights.x + vertex.Weights.y + vertex.Weights.z + vertex.Weights.w;
    if (totalWeight > 0.0) {  // Avoid division by zero
        vertex.Weights /= totalWeight;
    }
    else {
        // Handle the case where no weights are assigned (all are zero)
        vertex.Weights = glm::vec4(1.0, 0.0, 0.0, 0.0); // Default to only being influenced by the first bone
    }
}

std::vector<std::unique_ptr<RenderableNode>> ModelLoader::loadModel(const std::string& path, const std::string& materialPath) {

    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_ImproveCacheLocality);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        throw std::runtime_error("Failed to load model");
    }

    std::vector<std::unique_ptr<IRenderable>> renderables;
    std::vector<std::shared_ptr<Material>> materials;

    // Check if the materials for the given materialPath are already in the cache
    auto cacheIt = materialCache.find(materialPath);
    if (cacheIt != materialCache.end()) {
        // Materials found in cache, retrieve them
        materials = cacheIt->second;
    }
    else {
        // Materials not in cache, load them and add them to the cache
        materials = loadMaterials(materialPath);
        materialCache[materialPath] = materials;
    }

    std::vector<std::unique_ptr<RenderableNode>> renderableNodes;

    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[i];

        std::shared_ptr<Material> material = nullptr;
        if (!materials.empty()) {
            material = (i < materials.size()) ? materials[i] : materials.front();
        }

        std::unique_ptr<RenderableNode> renderableNode;
        if (mesh->HasBones()) {
            renderableNode = processAnimatedMesh(mesh, scene, material);
        }
        else {
            renderableNode = processStaticMesh(mesh, scene, material);
        }

        if (renderableNode) {
            renderableNodes.push_back(std::move(renderableNode));
        }
    }

    return renderableNodes;
}

std::vector<std::shared_ptr<Material>> ModelLoader::loadMaterials(const std::string& materialPath) {
    std::vector<std::shared_ptr<Material>> materials;

    // Check file extension
    std::string extension = getFileExtension(materialPath);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower); // Ensure extension is lowercase

    //std::cout << "Loading materials from file: " << materialPath << " (extension: " << extension << ")" << std::endl;

    if (extension == ".txt") {
        // It's a material list file
        auto materialFiles = readMaterialList(materialPath);
        //std::cout << "Read " << materialFiles.size() << " material files from list" << std::endl;

        for (const auto& materialFile : materialFiles) {
            //std::cout << "Parsing material file: " << materialFile << std::endl;
            materials.push_back(std::make_shared<Material>(MaterialParser::parseMaterialXML(materialFile)));
        }
    }
    else if (extension == ".xml") {
        // It's a single material file
        //std::cout << "Parsing single material file: " << materialPath << std::endl;
        materials.push_back(std::make_shared<Material>(MaterialParser::parseMaterialXML(materialPath)));
    }

    //std::cout << "Loaded " << materials.size() << " materials" << std::endl;

    return materials;
}

std::unique_ptr<RenderableNode> ModelLoader::processStaticMesh(aiMesh* mesh, const aiScene* scene, std::shared_ptr<Material> material) {
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

            DEBUG_COUT << "Cubemap faces order from processStaticMesh:" << std::endl;
            for (const auto& [faceName, path] : cubemapFaces) {
                std::string fullPath = FileSystemUtils::getAssetFilePath("textures/" + path);
                paths.push_back(fullPath); // Collect all paths for the cubemap

                DEBUG_COUT << "Face: " << faceName << ", Path: " << fullPath << std::endl;
            }

            // Use the new method to get or create a cubemap from the paths
            Texture cubemapTexture = TextureLoader::createCubemap(paths);
            if (cubemapTexture.id != 0) { // Check if texture was successfully created or retrieved from cache
                cubemapTexture.type = "environment";
                textures.push_back(cubemapTexture); // Push back the created or cached cubemap texture to the geometry's textures list
                DEBUG_COUT << "Cubemap texture added to textures vector. Type: " << cubemapTexture.type << ", ID: " << cubemapTexture.id << std::endl;
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

    auto geometry = std::make_unique<StaticGeometry>(vertices, indices, textures);
    geometry->setMaterial(material);

    return std::make_unique<RenderableNode>(mesh->mName.C_Str(), std::move(geometry));
}

std::unique_ptr<RenderableNode> ModelLoader::processAnimatedMesh(aiMesh* mesh, const aiScene* scene, std::shared_ptr<Material> material) {
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

        // Check and handle tangents and bitangents
        if (mesh->HasTangentsAndBitangents()) {
            vertex.Tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
            vertex.Bitangent = glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
            DEBUG_COUT << "[Info] Vertex " << i << " tangent: (" << vertex.Tangent.x << ", " << vertex.Tangent.y << ", " << vertex.Tangent.z << ")" << std::endl;
            DEBUG_COUT << "[Info] Vertex " << i << " bitangent: (" << vertex.Bitangent.x << ", " << vertex.Bitangent.y << ", " << vertex.Bitangent.z << ")" << std::endl;
        }
        else {
            vertex.Tangent = glm::vec3(0.0f);
            vertex.Bitangent = glm::vec3(0.0f);
            DEBUG_COUT << "[Info] Vertex " << i << " has no tangent/bitangent, assigning default (0, 0, 0)" << std::endl;
        }

        // Extract bone IDs and weights
        SetVertexBoneDataToDefault(vertex);
        ExtractBoneWeightForVertices(vertex, mesh, i);
        NormalizeVertexWeights(vertex);

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

            DEBUG_COUT << "Cubemap faces order from processAnimatedMesh:" << std::endl;
            for (const auto& [faceName, path] : cubemapFaces) {
                std::string fullPath = FileSystemUtils::getAssetFilePath("textures/" + path);
                paths.push_back(fullPath); // Collect all paths for the cubemap

                DEBUG_COUT << "Face: " << faceName << ", Path: " << fullPath << std::endl;
            }

            // Use the new method to get or create a cubemap from the paths
            Texture cubemapTexture = TextureLoader::createCubemap(paths);
            if (cubemapTexture.id != 0) { // Check if texture was successfully created or retrieved from cache
                cubemapTexture.type = "environment";
                textures.push_back(cubemapTexture); // Push back the created or cached cubemap texture to the geometry's textures list
                DEBUG_COUT << "Cubemap texture added to textures vector. Type: " << cubemapTexture.type << ", ID: " << cubemapTexture.id << std::endl;
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

    auto geometry = std::make_unique<AnimatedGeometry>(vertices, indices, textures, m_BoneInfoMap);
    geometry->setMaterial(material);

    auto renderableNode = std::make_unique<RenderableNode>(mesh->mName.C_Str(), std::move(geometry));

    return renderableNode;
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
    vertex.BoneIDs = glm::ivec4(-1);
    vertex.Weights = glm::vec4(0.0f);
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
    for (int i = 0; i < 4; ++i) {
        if (vertex.BoneIDs[i] < 0) {
            vertex.Weights[i] = weight;
            vertex.BoneIDs[i] = boneID;
            break;
        }
    }
}

const std::map<std::string, BoneInfo>& ModelLoader::getBoneInfoMap() {
    return m_BoneInfoMap;
}


