#pragma once
#include <map>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "geometry/SkinnedVertex.h"
#include "geometry/Skeleton.h"
#include "geometry/Animation.h"
#include "geometry/Bone.h"
#include "shader.h"
#include "utilities/MathUtils.h"
#include "utilities/OpenGLUtils.h"

struct BoneInfo {
    glm::mat4 BoneOffset; // Transforms from model space to bone space
    glm::mat4 FinalTransformation; // Final transformation of the bone after applying animation

    BoneInfo() : BoneOffset(glm::mat4(1.0f)), FinalTransformation(glm::mat4(1.0f)) {}
};

class SkinnedMesh {
public:
    SkinnedMesh();
    ~SkinnedMesh();

    void processMesh(aiMesh* mesh, const aiScene* scene);
    void render(const Shader& shader, const glm::mat4& modelMatrix, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) const;
    void initFromVectors(std::vector<glm::vec3>& positions, std::vector<glm::vec3>& normals,
        std::vector<glm::vec2>& texCoords, std::vector<unsigned int>& indices,
        std::vector<glm::vec4>& weights, std::vector<glm::ivec4>& boneIDs);

private:
    GLuint VAO, VBO, EBO;
    unsigned int numIndices = 0;
    std::vector<BoneInfo> boneInfo; // Stores information for each bone
    std::map<std::string, unsigned int> boneMapping; // Maps a bone name to its index
    unsigned int numBones = 0; // Tracks the number of unique bones

    void setupMesh(const std::vector<SkinnedVertex>& vertices, const std::vector<unsigned int>& indices);
};
