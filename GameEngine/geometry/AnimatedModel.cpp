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

    // Create skeleton instance
    skeleton = std::make_shared<Skeleton>();

    // Process bones and hierarchy
    processBonesAndHierarchy(scene, skeleton);

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
    CheckGLErrors("After shader use");
    shader.setMat4("view", viewMatrix);
    CheckGLErrors("After shader use");
    shader.setMat4("projection", projectionMatrix);
    CheckGLErrors("After setting projection matrix");

    if (skinnedMeshes.empty()) {
        std::cerr << "No skinned meshes to draw." << std::endl;
    }

    // Assuming each SkinnedMesh can use the same view and projection but has its own model matrix
    for (const auto& mesh : skinnedMeshes) {
        glm::mat4 modelMatrix = glm::mat4(1.0f); // Replace with actual model matrix for each mesh
        mesh->passBoneTransformationsToShader(shader);
        CheckGLErrors("After passing bone transformations");
        mesh->render(shader, modelMatrix, viewMatrix, projectionMatrix);
        CheckGLErrors("After rendering mesh");
    }
}

void AnimatedModel::setAnimation(const std::string& animationName) {
    auto animIt = animations.find(animationName);
    if (animIt != animations.end()) {
        currentAnimation = animIt->second;
        currentAnimationTime = 0.0f; // Reset the animation time
    }
    else {
        std::cerr << "Animation " << animationName << " not found." << std::endl;
    }
}

void AnimatedModel::update(float deltaTime) {
    if (!currentAnimation) return;

    float ticksPerFrame = currentAnimation->ticksPerSecond * deltaTime;
    currentAnimationTime += ticksPerFrame;

    // Loop the animation
    if (currentAnimationTime > currentAnimation->duration) {
        currentAnimationTime = fmod(currentAnimationTime, currentAnimation->duration);
    }

    // A map to store the final transformations for each bone based on the current animation frame
    std::map<std::string, glm::mat4> boneTransforms;

    // For each bone in the animation, calculate its final transformation
    for (const auto& channelPair : currentAnimation->channels) {
        const std::string& boneName = channelPair.first;
        const AnimationChannel& channel = channelPair.second;

        glm::mat4 boneTransform = glm::mat4(1.0f); // Start with identity matrix

        // Interpolate position, rotation, and scale based on the current animation time
        glm::vec3 interpolatedPosition = currentAnimation->interpolatePosition(currentAnimationTime, channel);
        glm::quat interpolatedRotation = currentAnimation->interpolateRotation(currentAnimationTime, channel);
        glm::vec3 interpolatedScale = currentAnimation->interpolateScale(currentAnimationTime, channel);

        // Construct the transformation matrix from the interpolated position, rotation, and scale
        glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), interpolatedPosition);
        glm::mat4 rotationMatrix = glm::mat4_cast(interpolatedRotation);
        glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), interpolatedScale);

        boneTransform = translationMatrix * rotationMatrix * scaleMatrix;

        // Store the final transformation for this bone
        boneTransforms[boneName] = boneTransform;
    }

    // Update the skeleton's bone matrices based on the calculated transformations
    skeleton->updateBoneMatricesFromAnimation(boneTransforms);

    // Update the SkinnedMeshes if necessary. This step requires that SkinnedMeshes have a way to receive updated bone transformations.
    // This might involve, for example, updating a uniform buffer with the new bone matrices.
    for (auto& mesh : skinnedMeshes) {
        mesh->updateBoneTransforms(shader, skeleton->getBoneMatrices());
    }
}

void AnimatedModel::processBonesAndHierarchy(const aiScene* scene, std::shared_ptr<Skeleton> skeleton) {
    // Temporary mapping to find bones quickly when setting up the hierarchy
    std::unordered_map<std::string, std::shared_ptr<Bone>> tempBoneMap;

    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[i];
        for (unsigned int j = 0; j < mesh->mNumBones; j++) {
            aiBone* aiBone = mesh->mBones[j];
            std::string boneName = aiBone->mName.C_Str();

            // Avoid creating duplicate Bone instances
            if (tempBoneMap.find(boneName) == tempBoneMap.end()) {
                // Providing a default ID value of -1
                std::shared_ptr<Bone> bone = std::make_shared<Bone>(boneName, -1, MathUtils::convertMatrixToGLMFormat(aiBone->mOffsetMatrix));
                skeleton->addBone(bone);
                tempBoneMap[boneName] = bone;
            }
        }
    }

    // Now, establish the hierarchy by traversing the scene's node tree
    establishHierarchy(scene->mRootNode, nullptr, skeleton, tempBoneMap);
}

void AnimatedModel::establishHierarchy(const aiNode* node, std::shared_ptr<Bone> parentBone, std::shared_ptr<Skeleton> skeleton, std::unordered_map<std::string, std::shared_ptr<Bone>>& tempBoneMap) {
    std::string nodeName = node->mName.C_Str();
    std::shared_ptr<Bone> currentBone = nullptr;

    // Check if this node corresponds to a bone
    if (tempBoneMap.find(nodeName) != tempBoneMap.end()) {
        currentBone = tempBoneMap[nodeName];
        if (parentBone != nullptr) {
            currentBone->setParent(parentBone);
            parentBone->addChild(currentBone);
        }
    }
    else {
        currentBone = parentBone; // If not a bone, continue hierarchy traversal with the current parent bone
    }

    // Recursively establish hierarchy for child nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        establishHierarchy(node->mChildren[i], currentBone, skeleton, tempBoneMap);
    }
}



