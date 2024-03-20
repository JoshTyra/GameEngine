#include "SkinnedMesh.h"

void CheckGLErrors(const std::string& label) {
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL Error at " << label << ": " << err << std::endl;
    }
}

SkinnedMesh::SkinnedMesh() {
}

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

    std::cout << "Successfully loaded mesh file: " << filename << std::endl;

    // Process all meshes (or the first mesh, based on your needs) found
    // For simplicity, this example processes only the first mesh
    aiMesh* mesh = scene->mMeshes[0]; // Assuming there's at least one mesh
    processMesh(mesh, scene);

    return true; // Indicate success
}

void SkinnedMesh::setupMesh(const std::vector<SkinnedVertex>& vertices, const std::vector<unsigned int>& indices) {
    glGenVertexArrays(1, &VAO);
    CheckGLErrors("glGenVertexArrays");
    glGenBuffers(1, &VBO);
    CheckGLErrors("glGenBuffers");
    glGenBuffers(1, &EBO);
    CheckGLErrors("glGenBuffers");

    glBindVertexArray(VAO);
    CheckGLErrors("glBindVertexArray");

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    CheckGLErrors("glBindBuffer");
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(SkinnedVertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    CheckGLErrors("glGenBuffer");
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // Vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SkinnedVertex), (void*)offsetof(SkinnedVertex, Position));

    // Vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(SkinnedVertex), (void*)offsetof(SkinnedVertex, Normal));

    // Vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(SkinnedVertex), (void*)offsetof(SkinnedVertex, TexCoords));

    // Vertex bone ids
    glEnableVertexAttribArray(3);
    glVertexAttribIPointer(3, 4, GL_INT, sizeof(SkinnedVertex), (void*)offsetof(SkinnedVertex, BoneIDs));

    // Vertex bone weights
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(SkinnedVertex), (void*)offsetof(SkinnedVertex, Weights));

    glBindVertexArray(0);
    CheckGLErrors("End of setupMesh");
}

void SkinnedMesh::processMesh(aiMesh* mesh, const aiScene* scene) {
    std::vector<SkinnedVertex> vertices;
    std::vector<unsigned int> indices;

    std::cout << "Processing mesh with " << mesh->mNumVertices << " vertices and " << mesh->mNumFaces << " faces." << std::endl;

    // Process vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        SkinnedVertex vertex;
        // Positions
        vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        // Normals
        vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        // Texture Coordinates
        if (mesh->mTextureCoords[0]) { // Does the mesh contain texture coordinates?
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

    numIndices = static_cast<unsigned int>(indices.size());
    std::cout << "numIndices: " << numIndices << std::endl;

    // Process bones
    for (unsigned int i = 0; i < mesh->mNumBones; i++) {
        std::string boneName = mesh->mBones[i]->mName.C_Str();
        unsigned int boneIndex = 0;

        if (boneMapping.find(boneName) == boneMapping.end()) {
            // Allocate an index for a new bone, adding it to the mapping and bone info vector
            boneIndex = static_cast<unsigned int>(boneInfo.size());
            BoneInfo bi;
            bi.BoneOffset = convertMatrixToGLMFormat(mesh->mBones[i]->mOffsetMatrix);
            boneInfo.push_back(bi);
            boneMapping[boneName] = boneIndex;
        }
        else {
            boneIndex = boneMapping[boneName];
        }

        for (unsigned int j = 0; j < mesh->mBones[i]->mNumWeights; j++) {
            unsigned int vertexID = mesh->mBones[i]->mWeights[j].mVertexId;
            float weight = mesh->mBones[i]->mWeights[j].mWeight;
            vertices[vertexID].addBoneData(boneIndex, weight);
        }
    }

    std::cout << "Setting up mesh with VAO, VBO, EBO." << std::endl;

    // After vertices, indices, and bones have been processed, log the totals
    std::cout << "Vertices and indices processed. Total vertices: " << vertices.size() << ", Total indices: " << indices.size() << std::endl;

    // Setup mesh (VAO, VBO, EBO)
    setupMesh(vertices, indices);
}

void SkinnedMesh::passBoneTransformationsToShader(const Shader& shader) const {
    // Make sure your shader is using the correct uniform name ("boneTransforms" in this case)
    for (size_t i = 0; i < boneInfo.size(); ++i) {
        shader.setMat4("boneTransforms[" + std::to_string(i) + "]", boneInfo[i].FinalTransformation);
    }
}

void SkinnedMesh::render(const Shader& shader, const glm::mat4& modelMatrix, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) const {
    // Bind the VAO and draw the mesh
    glBindVertexArray(VAO);
    CheckGLErrors("glBindVertexArray");

    shader.use(); // Activate the shader program
    CheckGLErrors("Shader use");

    // Pass the model, view, and projection matrices to the shader
    shader.setMat4("model", modelMatrix);
    shader.setMat4("view", viewMatrix);
    shader.setMat4("projection", projectionMatrix);

    // Upload bone transformations to the shader
    passBoneTransformationsToShader(shader);

    // Debug check: Get the currently bound VAO before drawing
    GLint currentlyBoundVAO;
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &currentlyBoundVAO);
    std::cout << "Currently bound VAO: " << currentlyBoundVAO << std::endl;
    if (currentlyBoundVAO != VAO) {
        std::cerr << "Unexpected VAO bound! Expected: " << VAO << ", got: " << currentlyBoundVAO << std::endl;
        // Optionally bind the correct VAO if it's not already bound
        // glBindVertexArray(VAO);
        // CheckGLErrors("glBindVertexArray");
    }

    std::cout << "Drawing elements. Count: " << static_cast<GLsizei>(numIndices) << std::endl;
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(numIndices), GL_UNSIGNED_INT, 0);
    CheckGLErrors("glDrawElements");

    glBindVertexArray(0);
    CheckGLErrors("glBindVertexArray unbind");
}




