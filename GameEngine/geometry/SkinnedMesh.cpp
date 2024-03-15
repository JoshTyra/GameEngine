#include "SkinnedMesh.h"

SkinnedMesh::~SkinnedMesh() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

glm::mat4 SkinnedMesh::convertMatrixToGLMFormat(const aiMatrix4x4& from) {
    glm::mat4 to;
    // Copy the data from 'from' to 'to', taking care to transpose or rearrange as necessary
    to[0][0] = from.a1; to[1][0] = from.b1; to[2][0] = from.c1; to[3][0] = from.d1;
    to[0][1] = from.a2; to[1][1] = from.b2; to[2][1] = from.c2; to[3][1] = from.d2;
    to[0][2] = from.a3; to[1][2] = from.b3; to[2][2] = from.c3; to[3][2] = from.d3;
    to[0][3] = from.a4; to[1][3] = from.b4; to[2][3] = from.c4; to[3][3] = from.d4;
    return to;
}

bool SkinnedMesh::loadMesh(const std::string& filename) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_LimitBoneWeights);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return false; // Indicate failure
    }

    // Process all meshes (or the first mesh, based on your needs) found
    // For simplicity, this example processes only the first mesh
    aiMesh* mesh = scene->mMeshes[0]; // Assuming there's at least one mesh
    processMesh(mesh, scene);

    return true; // Indicate success
}

void SkinnedMesh::setupMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // Vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
    // Vertex Normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // Vertex Texture Coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    // Vertex Bone IDs
    glEnableVertexAttribArray(3);
    glVertexAttribIPointer(3, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, BoneIDs));
    // Vertex Weights
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Weights));

    glBindVertexArray(0);
}

void SkinnedMesh::processMesh(aiMesh* mesh, const aiScene* scene) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // Process vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        // Positions
        vertex.Position.x = mesh->mVertices[i].x;
        vertex.Position.y = mesh->mVertices[i].y;
        vertex.Position.z = mesh->mVertices[i].z;
        // Normals
        vertex.Normal.x = mesh->mNormals[i].x;
        vertex.Normal.y = mesh->mNormals[i].y;
        vertex.Normal.z = mesh->mNormals[i].z;
        // Texture Coordinates
        if (mesh->mTextureCoords[0]) { // Does the mesh contain texture coordinates?
            vertex.TexCoords.x = mesh->mTextureCoords[0][i].x;
            vertex.TexCoords.y = mesh->mTextureCoords[0][i].y;
        }
        else {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }
        // Bone IDs and Weights (initialized to 0 in the Vertex struct)
        vertices.push_back(vertex);
    }

    // Process indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // Process bones
    for (unsigned int i = 0; i < mesh->mNumBones; i++) {
        unsigned int BoneIndex = 0;
        std::string BoneName(mesh->mBones[i]->mName.data);

        if (boneMapping.find(BoneName) == boneMapping.end()) {
            BoneIndex = numBones;
            numBones++;
            BoneInfo bi;
            bi.BoneOffset = convertMatrixToGLMFormat(mesh->mBones[i]->mOffsetMatrix);
            boneInfo.push_back(bi);
            boneMapping[BoneName] = BoneIndex;
        }
        else {
            BoneIndex = boneMapping[BoneName];
        }

        for (unsigned int j = 0; j < mesh->mBones[i]->mNumWeights; j++) {
            unsigned int VertexID = mesh->mBones[i]->mWeights[j].mVertexId;
            float Weight = mesh->mBones[i]->mWeights[j].mWeight;
            vertices[VertexID].addBoneData(BoneIndex, Weight);
        }
    }

    // Assuming you have a method setupMesh() to setup VAO, VBO, etc.
    setupMesh(vertices, indices);
}

