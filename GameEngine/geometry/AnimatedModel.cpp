#include "AnimatedModel.h"

AnimatedModel::AnimatedModel(const std::string& vertexPath, const std::string& fragmentPath) : shader(vertexPath, fragmentPath) {
    this->shader = Shader(vertexPath, fragmentPath);
}

AnimatedModel::~AnimatedModel() {}

// Converts an Assimp aiVector3D to a GLM vec3
glm::vec3 AssimpToGLMVec3(const aiVector3D& vec) {
    return glm::vec3(vec.x, vec.y, vec.z);
}

// Converts an Assimp aiQuaternion to a GLM quat
glm::quat AssimpToGLMQuat(const aiQuaternion& quat) {
    return glm::quat(quat.w, quat.x, quat.y, quat.z); // Note the order of parameters: w, x, y, z
}

bool AnimatedModel::loadModel(const std::string& path) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_LimitBoneWeights);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return false;
    }

    // Process each mesh
    skinnedMeshes.reserve(scene->mNumMeshes);

    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        std::shared_ptr<SkinnedMesh> mesh = std::make_shared<SkinnedMesh>();
        mesh->processMesh(scene->mMeshes[i], scene);
        skinnedMeshes.push_back(mesh);
    }

    // Load animations
    loadAnimations(scene);

    return true;
}

void AnimatedModel::loadAnimations(const aiScene* scene) {
    std::cout << "Loading animations:" << std::endl;
    for (unsigned int i = 0; i < scene->mNumAnimations; i++) {
        std::string animName = scene->mAnimations[i]->mName.C_Str();
        std::cout << " - " << animName << std::endl;
        aiAnimation* aiAnim = scene->mAnimations[i];
        Animation animation(aiAnim->mName.C_Str(), static_cast<float>(aiAnim->mDuration), static_cast<float>(aiAnim->mTicksPerSecond));

        for (unsigned int j = 0; j < aiAnim->mNumChannels; j++) {
            aiNodeAnim* channel = aiAnim->mChannels[j];
            AnimationChannel animChannel(channel->mNodeName.C_Str());

            // Process position keyframes
            for (unsigned int k = 0; k < channel->mNumPositionKeys; k++) {
                aiVectorKey key = channel->mPositionKeys[k];
                Keyframe keyframe(static_cast<float>(key.mTime), AssimpToGLMVec3(key.mValue), glm::quat(), glm::vec3(1.0f));
                animChannel.keyframes.push_back(keyframe);
            }

            // Process rotation keyframes
            for (unsigned int k = 0; k < channel->mNumRotationKeys; k++) {
                aiQuatKey key = channel->mRotationKeys[k];
                Keyframe keyframe(static_cast<float>(key.mTime), glm::vec3(), AssimpToGLMQuat(key.mValue), glm::vec3(1.0f));
                animChannel.keyframes.push_back(keyframe);
            }

            // Process scaling keyframes
            for (unsigned int k = 0; k < channel->mNumScalingKeys; k++) {
                aiVectorKey key = channel->mScalingKeys[k];
                Keyframe keyframe(static_cast<float>(key.mTime), glm::vec3(), glm::quat(), AssimpToGLMVec3(key.mValue));
                animChannel.keyframes.push_back(keyframe);
            }

            animation.addChannel(animChannel);
        }

        animations[animName] = std::make_shared<Animation>(animation); // Use animName from aiAnimation
    }
}

void AnimatedModel::draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) const {
    shader.use();
    shader.setMat4("view", viewMatrix);
    shader.setMat4("projection", projectionMatrix);

    // Assuming each SkinnedMesh can use the same view and projection but has its own model matrix
    for (const auto& mesh : skinnedMeshes) {
        glm::mat4 modelMatrix = glm::mat4(1.0f); // Replace with actual model matrix for each mesh
        mesh->passBoneTransformationsToShader(shader);
        mesh->render(shader, modelMatrix, viewMatrix, projectionMatrix);
    }
}

void AnimatedModel::setAnimation(const std::string& animationName) {
    auto animIt = animations.find(animationName);
    if (animIt != animations.end()) {
        currentAnimation = animIt->second;
    }
    else {
        std::cerr << "Animation " << animationName << " not found." << std::endl;
    }
}

void AnimatedModel::update(float deltaTime) {
    if (!currentAnimation) return;

    // Update current animation time
    // Adjust this logic based on how you've structured Animation and how time is tracked
    // For example, consider looping or clamping animation playback

    // Update bone transformations in the skeleton based on the currentAnimation and currentTime
}
