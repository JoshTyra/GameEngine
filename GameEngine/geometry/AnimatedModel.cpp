#include "AnimatedModel.h"

AnimatedModel::AnimatedModel(const std::string& vertexPath, const std::string& fragmentPath)
    : shader(vertexPath, fragmentPath), position(0.0f), rotation(glm::quat(1.0, 0.0, 0.0, 0.0)), scale(1.0f) {
    createBoneMatricesBuffer();
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
    //processBonesAndHierarchy(scene, skeleton);

    // Load animations
    //loadAnimations(scene);

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

    // At this point, the skeleton's boneMatrices vector is updated and ready for use in rendering
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
    glGenBuffers(1, &uboBoneMatrices);
    CheckGLErrors("glGenBuffers");

    glBindBuffer(GL_UNIFORM_BUFFER, uboBoneMatrices);
    CheckGLErrors("glBindBuffer GL_UNIFORM_BUFFER");

    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * MAX_BONES, nullptr, GL_DYNAMIC_DRAW);
    CheckGLErrors("glBufferData");

    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    CheckGLErrors("glBindBuffer 0");

    // Ensure the shader program is being used before getting and setting the uniform block index.
    shader.use();
    CheckGLErrors("shader.use");

    GLuint blockIndex = glGetUniformBlockIndex(shader.Program, "BoneMatrices");
    if (blockIndex == GL_INVALID_INDEX) {
        std::cerr << "Uniform Block 'BoneMatrices' not found!" << std::endl;
    }
    else {
        glUniformBlockBinding(shader.Program, blockIndex, BONE_MATRICES_BINDING_POINT);
        CheckGLErrors("glUniformBlockBinding");

        glBindBufferBase(GL_UNIFORM_BUFFER, BONE_MATRICES_BINDING_POINT, uboBoneMatrices);
        CheckGLErrors("glBindBufferBase");
    }
}

void AnimatedModel::updateBoneMatrices(const std::vector<glm::mat4>& boneMatrices) {
    shader.use();
    glBindBuffer(GL_UNIFORM_BUFFER, uboBoneMatrices);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4) * boneMatrices.size(), boneMatrices.data());
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

glm::mat4 AnimatedModel::getModelMatrix() const {
    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), position);
    glm::mat4 rotationMatrix = glm::mat4_cast(rotation);
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale);
    return translationMatrix * rotationMatrix * scaleMatrix;
}

void AnimatedModel::draw(const RenderingContext& context) const {
    shader.use();
    shader.setMat4("view", context.viewMatrix);
    shader.setMat4("projection", context.projectionMatrix);
    glm::mat4 modelMatrix = getModelMatrix();
    shader.setMat4("model", modelMatrix);

    // Check if bone matrices binding point is specified and bind the bone matrices UBO to it before drawing
    if (context.boneMatricesBindingPoint != 0) {
        glBindBufferBase(GL_UNIFORM_BUFFER, context.boneMatricesBindingPoint, uboBoneMatrices);
    }

    for (const auto& mesh : skinnedMeshes) {
        mesh->render(shader, modelMatrix, context.viewMatrix, context.projectionMatrix);
    }

    // Optionally, if you unbind the bone matrices UBO after drawing,
    // make sure to check if it was bound in the first place
    if (context.boneMatricesBindingPoint != 0) {
        glBindBufferBase(GL_UNIFORM_BUFFER, context.boneMatricesBindingPoint, 0);
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



