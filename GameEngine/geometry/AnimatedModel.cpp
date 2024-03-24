#include "AnimatedModel.h"

AnimatedModel::AnimatedModel(const std::string& vertexPath, const std::string& fragmentPath)
    : shader(vertexPath, fragmentPath), position(0.0f), scale(1.0f) {
    // Apply a rotation of -90 degrees around the X-axis
    rotation = glm::quat(glm::vec3(glm::radians(-90.0f), 0.0f, 0.0f));
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
    //loadAnimations(scene);

    //createBoneMatricesBuffer();

    updateShaderWithBoneMatrices();

    return true;
}

void AnimatedModel::loadAnimations(const aiScene* scene) {
    std::cout << "Loading animations:" << std::endl;
    for (unsigned int i = 0; i < scene->mNumAnimations; i++) {
        aiAnimation* aiAnim = scene->mAnimations[i];
        std::string animName = aiAnim->mName.C_Str();
        std::cout << " - " << animName
            << " | Channels: " << aiAnim->mNumChannels
            << " | Duration: " << aiAnim->mDuration
            << " | TicksPerSecond: " << aiAnim->mTicksPerSecond << std::endl;

        Animation animation(aiAnim->mName.C_Str(), static_cast<float>(aiAnim->mDuration), static_cast<float>(aiAnim->mTicksPerSecond));

        for (unsigned int j = 0; j < aiAnim->mNumChannels; j++) {
            aiNodeAnim* channel = aiAnim->mChannels[j];
            std::cout << "Channel: " << channel->mNodeName.C_Str()
                << " | PositionKeys: " << channel->mNumPositionKeys
                << " | RotationKeys: " << channel->mNumRotationKeys
                << " | ScalingKeys: " << channel->mNumScalingKeys << std::endl;

            AnimationChannel animChannel(channel->mNodeName.C_Str());

            if (channel->mNumPositionKeys > 1) {
                // Safe to access the first and last key if there are more than one keyframes
                const aiVectorKey& firstPosKey = channel->mPositionKeys[0];
                const aiVectorKey& lastPosKey = channel->mPositionKeys[channel->mNumPositionKeys - 1];
                std::cout << "First Position Key Time: " << firstPosKey.mTime << " Value: "
                    << firstPosKey.mValue.x << ", " << firstPosKey.mValue.y << ", " << firstPosKey.mValue.z << std::endl;
                std::cout << "Last Position Key Time: " << lastPosKey.mTime << " Value: "
                    << lastPosKey.mValue.x << ", " << lastPosKey.mValue.y << ", " << lastPosKey.mValue.z << std::endl;
            }
            else if (channel->mNumPositionKeys == 1) {
                // If there's exactly one keyframe, it's both the first and the last
                const aiVectorKey& key = channel->mPositionKeys[0];
                std::cout << "Only Position Key Time: " << key.mTime << " Value: "
                    << key.mValue.x << ", " << key.mValue.y << ", " << key.mValue.z << std::endl;
            }

            if (channel->mNumRotationKeys > 1) {
                // Safe to access the first and last key if there are more than one keyframes
                const aiQuatKey& firstRotKey = channel->mRotationKeys[0];
                const aiQuatKey& lastRotKey = channel->mRotationKeys[channel->mNumRotationKeys - 1];
                std::cout << "First Rotation Key Time: " << firstRotKey.mTime << " Value: "
                    << firstRotKey.mValue.x << ", " << firstRotKey.mValue.y << ", "
                    << firstRotKey.mValue.z << ", " << firstRotKey.mValue.w << std::endl;
                std::cout << "Last Rotation Key Time: " << lastRotKey.mTime << " Value: "
                    << lastRotKey.mValue.x << ", " << lastRotKey.mValue.y << ", "
                    << lastRotKey.mValue.z << ", " << lastRotKey.mValue.w << std::endl;
            }
            else if (channel->mNumRotationKeys == 1) {
                // If there's exactly one keyframe, it's both the first and the last
                const aiQuatKey& key = channel->mRotationKeys[0];
                std::cout << "Only Rotation Key Time: " << key.mTime << " Value: "
                    << key.mValue.x << ", " << key.mValue.y << ", "
                    << key.mValue.z << ", " << key.mValue.w << std::endl;
            }

            if (channel->mNumScalingKeys > 1) {
                // Safe to access the first and last key if there are more than one keyframe
                const aiVectorKey& firstScaleKey = channel->mScalingKeys[0];
                const aiVectorKey& lastScaleKey = channel->mScalingKeys[channel->mNumScalingKeys - 1];
                std::cout << "First Scaling Key Time: " << firstScaleKey.mTime << " Value: "
                    << firstScaleKey.mValue.x << ", " << firstScaleKey.mValue.y << ", " << firstScaleKey.mValue.z << std::endl;
                std::cout << "Last Scaling Key Time: " << lastScaleKey.mTime << " Value: "
                    << lastScaleKey.mValue.x << ", " << lastScaleKey.mValue.y << ", " << lastScaleKey.mValue.z << std::endl;
            }
            else if (channel->mNumScalingKeys == 1) {
                // If there's exactly one keyframe, it's both the first and the last
                const aiVectorKey& key = channel->mScalingKeys[0];
                std::cout << "Only Scaling Key Time: " << key.mTime << " Value: "
                    << key.mValue.x << ", " << key.mValue.y << ", " << key.mValue.z << std::endl;
            }

            animation.addChannel(animChannel);
        }

        animations[animName] = std::make_shared<Animation>(animation); // Use animName from aiAnimation
    }
}

void AnimatedModel::update(float deltaTime) {
    //if (!currentAnimation) return;

    //// Update the current animation time
    //float ticksPerFrame = currentAnimation->ticksPerSecond * deltaTime;
    //currentAnimationTime += ticksPerFrame;
    //if (currentAnimationTime > currentAnimation->duration) {
    //    currentAnimationTime = fmod(currentAnimationTime, currentAnimation->duration);
    //}

    //// Calculate the final transformations for each bone
    //std::map<std::string, glm::mat4> boneTransforms;
    //for (const auto& bone : skeleton->getBones()) {
    //    glm::mat4 boneTransform = currentAnimation->getBoneTransform(currentAnimationTime, bone->getName());
    //    boneTransforms[bone->getName()] = boneTransform;
    //}

    //// Update the skeleton with these transformations
    //skeleton->updateBoneMatricesFromAnimation(boneTransforms);
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

void AnimatedModel::createBoneMatricesBuffer() {
    // Assuming MAX_BONES is defined somewhere
    std::vector<glm::mat4> identityMatrices(MAX_BONES, glm::mat4(1.0f)); // Identity matrices

    glGenBuffers(1, &uboBoneMatrices);
    glBindBuffer(GL_UNIFORM_BUFFER, uboBoneMatrices);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * MAX_BONES, identityMatrices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // Ensure the shader program is being used before getting and setting the uniform block index.
    shader.use();
    CheckGLErrors("shader.use");

    shader.use(); // Ensure shader program is active

    const std::vector<glm::mat4>& boneMatrices = skeleton->getBoneMatrices();
    // Log the first bone matrix, or loop through more if needed
    if (!boneMatrices.empty()) {
        const glm::mat4& firstBoneMatrix = boneMatrices[0];
        std::cout << "First bone matrix: \n";
        for (int i = 0; i < 4; ++i) {
            std::cout << "["
                << firstBoneMatrix[i][0] << ", "
                << firstBoneMatrix[i][1] << ", "
                << firstBoneMatrix[i][2] << ", "
                << firstBoneMatrix[i][3] << "]\n";
        }
    }

    for (size_t i = 0; i < boneMatrices.size(); ++i) {
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, ("bones[" + std::to_string(i) + "]").c_str()),
            1, GL_FALSE, glm::value_ptr(boneMatrices[i]));
    }
}

void AnimatedModel::updateShaderWithBoneMatrices() {
    shader.use(); // Ensure the shader program is active

    // Assuming `skeleton` is valid and contains the current bone matrices
    const std::vector<glm::mat4>& boneMatrices = skeleton->getBoneMatrices();

    // For debugging, you can log the first bone matrix as you have done
    if (!boneMatrices.empty()) {
        const glm::mat4& firstBoneMatrix = boneMatrices[0];
        std::cout << "First bone matrix: \n";
        for (int i = 0; i < 4; ++i) {
            std::cout << "["
                << firstBoneMatrix[i][0] << ", "
                << firstBoneMatrix[i][1] << ", "
                << firstBoneMatrix[i][2] << ", "
                << firstBoneMatrix[i][3] << "]\n";
        }
    }

    // Update shader with each bone matrix
    for (size_t i = 0; i < boneMatrices.size(); ++i) {
        std::string uniformName = "bones[" + std::to_string(i) + "]";
        GLint loc = glGetUniformLocation(shader.Program, uniformName.c_str());
        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(boneMatrices[i]));
    }
}


void AnimatedModel::updateBoneMatrices(const std::vector<glm::mat4>& boneMatrices) {
    shader.use();
    glBindBuffer(GL_UNIFORM_BUFFER, uboBoneMatrices);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4) * boneMatrices.size(), boneMatrices.data());
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

glm::mat4 AnimatedModel::getModelMatrix() const {
    glm::mat4 model = glm::mat4(1.0f);
    // Apply scaling
    model = glm::scale(model, scale);
    // Apply rotation
    model *= glm::mat4_cast(rotation);
    // Apply translation
    model = glm::translate(model, position);
    return model;
}

void AnimatedModel::draw(const RenderingContext& context) const {
    shader.use();
    shader.setMat4("view", context.viewMatrix);
    shader.setMat4("projection", context.projectionMatrix);
    glm::mat4 modelMatrix = getModelMatrix();
    shader.setMat4("model", modelMatrix);

    // Temporarily bypass bone matrix logic for this test
    // if (context.boneMatricesBindingPoint != 0) {
    //     glBindBufferBase(GL_UNIFORM_BUFFER, context.boneMatricesBindingPoint, uboBoneMatrices);
    // }

    for (const auto& mesh : skinnedMeshes) {
        mesh->render(shader, modelMatrix, context.viewMatrix, context.projectionMatrix);
    }

    // Temporarily bypass bone matrix logic for this test
    // if (context.boneMatricesBindingPoint != 0) {
    //     glBindBufferBase(GL_UNIFORM_BUFFER, context.boneMatricesBindingPoint, 0);
    // }
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



