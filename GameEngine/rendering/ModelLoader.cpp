#include "ModelLoader.h"
#include "FileSystemUtils.h"

std::vector<std::unique_ptr<LevelGeometry>> ModelLoader::loadModel(const std::string& path, const std::string& materialListFile) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    // Handle errors in loading
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        throw std::runtime_error("Failed to load model");
    }

    std::vector<std::unique_ptr<LevelGeometry>> meshes;

    // Process each mesh in the scene
    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[i];

        // Process the mesh (vertices, indices)
        auto geometry = processMesh(mesh, scene);  // Assuming processMesh returns LevelGeometry

        meshes.push_back(std::move(geometry)); // Add the unique_ptr to the vector
    }

    return meshes; // Return the vector of unique_ptr to LevelGeometry
}

std::unique_ptr<LevelGeometry> ModelLoader::processMesh(aiMesh* mesh, const aiScene* scene) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // Process vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;

        // Process vertex positions
        if (mesh->HasPositions()) {
            vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        }

        // Process vertex normals
        if (mesh->HasNormals()) {
            vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        }

        // Process texture coordinates (if available)
        if (mesh->mTextureCoords[0]) { // Meshes can contain multiple sets of texture coordinates. We thus make the assumption that we won't use models where a vertex can have multiple texture coordinates.
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

    // Return a unique_ptr to the new LevelGeometry
    return std::make_unique<LevelGeometry>(std::move(vertices), std::move(indices));
}

