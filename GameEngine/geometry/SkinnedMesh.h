#include <map>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define MAX_BONE_INFLUENCE 4

struct BoneInfo {
    glm::mat4 BoneOffset;
    glm::mat4 FinalTransformation; // You might need this for the actual bone transformations during animation
};

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::ivec4 BoneIDs = glm::ivec4(0);
    glm::vec4 Weights = glm::vec4(0.0f);

    void addBoneData(unsigned int boneID, float weight) {
        for (int i = 0; i < MAX_BONE_INFLUENCE; ++i) {
            if (Weights[i] == 0.0) {
                BoneIDs[i] = boneID;
                Weights[i] = weight;
                return;
            }
        }
        std::cerr << "Vertex has more than " << MAX_BONE_INFLUENCE << " bone influences. Extra bones ignored." << std::endl;
    }
};

class SkinnedMesh {
public:
    SkinnedMesh();
    ~SkinnedMesh();

    bool loadMesh(const std::string& filename);
    void processMesh(aiMesh* mesh, const aiScene* scene);
    void render();
    void initFromVectors(std::vector<glm::vec3>& positions, std::vector<glm::vec3>& normals,
        std::vector<glm::vec2>& texCoords, std::vector<unsigned int>& indices,
        std::vector<glm::vec4>& weights, std::vector<glm::ivec4>& boneIDs);
    glm::mat4 convertMatrixToGLMFormat(const aiMatrix4x4& from);

private:
    GLuint VAO, VBO, EBO;
    unsigned int numIndices = 0;
    std::map<std::string, unsigned int> boneMapping; // Maps a bone name to its index
    unsigned int numBones = 0; // Tracks the number of unique bones
    std::vector<BoneInfo> boneInfo; // Stores the bone information

    void setupMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
};
