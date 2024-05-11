#include "AnimatedGeometry.h"

AnimatedGeometry::AnimatedGeometry()
	: VAO(0), VBO(0), EBO(0), shader(nullptr),
	wireframeCubeVAO(0), wireframeCubeVBO(0), wireframeCubeEBO(0),
	wireframeCubeShader(FileSystemUtils::getAssetFilePath("shaders/wireframe_cube.vert"),
		FileSystemUtils::getAssetFilePath("shaders/wireframe_cube.frag")) {
}

AnimatedGeometry::AnimatedGeometry(const std::vector<AnimatedVertex>& vertices,
	const std::vector<unsigned int>& indices,
	const std::vector<Texture>& textures,
	const std::map<std::string, BoneInfo>& boneInfoMap)
	: vertices(vertices), indices(indices), textures(textures),
	VAO(0), VBO(0), EBO(0), shader(nullptr),
	m_BoneInfoMap(boneInfoMap),
	wireframeCubeVAO(0), wireframeCubeVBO(0), wireframeCubeEBO(0),
	wireframeCubeShader(FileSystemUtils::getAssetFilePath("shaders/wireframe_cube.vert"),
		FileSystemUtils::getAssetFilePath("shaders/wireframe_cube.frag")) {

	//for (const auto& vertex : vertices) {
	//	std::cout << "Vertex position: " << vertex.Position.x << ", " << vertex.Position.y << ", " << vertex.Position.z << std::endl;
	//	std::cout << "Bone IDs: " << vertex.BoneIDs[0] << ", " << vertex.BoneIDs[1] << ", " << vertex.BoneIDs[2] << ", " << vertex.BoneIDs[3] << std::endl;
	//	std::cout << "Weights: " << vertex.Weights[0] << ", " << vertex.Weights[1] << ", " << vertex.Weights[2] << ", " << vertex.Weights[3] << std::endl;
	//}

	setupMesh();
	calculateAABB();
	generateWireframeCubeVertices();
	setupWireframeCube();
}

AnimatedGeometry::~AnimatedGeometry() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	glDeleteVertexArrays(1, &wireframeCubeVAO);
	glDeleteBuffers(1, &wireframeCubeVBO);
	glDeleteBuffers(1, &wireframeCubeEBO);
}

void AnimatedGeometry::setupMesh() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(AnimatedVertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// Vertex attributes
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(AnimatedVertex), (void*)offsetof(AnimatedVertex, Position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(AnimatedVertex), (void*)offsetof(AnimatedVertex, Normal));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(AnimatedVertex), (void*)offsetof(AnimatedVertex, TexCoords));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(AnimatedVertex), (void*)offsetof(AnimatedVertex, Tangent));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(AnimatedVertex), (void*)offsetof(AnimatedVertex, Bitangent));
	glEnableVertexAttribArray(5);
	glVertexAttribIPointer(5, 4, GL_INT, sizeof(AnimatedVertex), (void*)offsetof(AnimatedVertex, BoneIDs));  // Correct function for integer attributes
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(AnimatedVertex), (void*)offsetof(AnimatedVertex, Weights));

	glBindVertexArray(0);
}

void AnimatedGeometry::setupWireframeCube() {
	glGenVertexArrays(1, &wireframeCubeVAO);
	glGenBuffers(1, &wireframeCubeVBO);
	glGenBuffers(1, &wireframeCubeEBO);

	glBindVertexArray(wireframeCubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, wireframeCubeVBO);
	glBufferData(GL_ARRAY_BUFFER, wireframeCubeVertices.size() * sizeof(glm::vec3), wireframeCubeVertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wireframeCubeEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, wireframeCubeIndices.size() * sizeof(unsigned int), wireframeCubeIndices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

	glBindVertexArray(0);
}

void AnimatedGeometry::draw(const glm::mat4& transform, Animator* animator, const Frustum& frustum) {
	if (!shader || !shader->Program) {
		std::cerr << "Shader not set or invalid for geometry, cannot draw." << std::endl;
		return;
	}

	// Update the AABB based on the current animation pose
	updateAABB(animator);

	 //Perform frustum culling using the updated AABB
	if (!isInFrustum(frustum)) {
		return;
	}

	// Save the current OpenGL states
	GLint currentShaderProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &currentShaderProgram);
	GLint currentVAO;
	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &currentVAO);
	// Save other relevant states if necessary

	// Render the wireframe cube
	renderWireframeCube(transform, animator);

	// Restore the OpenGL states for the animated character rendering
	glUseProgram(currentShaderProgram);
	glBindVertexArray(currentVAO);

	// Restore other relevant states if necessary
	shader->use();


	GLenum error;
	while ((error = glGetError()) != GL_NO_ERROR) {
		std::cerr << "OpenGL Error before setting uniforms: " << error << std::endl;
	}

	if (material) {
		const Technique& technique = material->getTechniqueDetails();

		// Check and apply face culling state
		if (technique.enableFaceCulling) {
			glEnable(GL_CULL_FACE);
		}
		else {
			glDisable(GL_CULL_FACE);
		}

		// Apply blending state
		if (technique.blending.enabled) {
			glEnable(GL_BLEND);
			glBlendFunc(technique.blending.src, technique.blending.dest);
			glBlendEquation(technique.blending.equation);
		}
		else {
			glDisable(GL_BLEND);
		}

		if (technique.enableDepthTest) {
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(technique.depthFunc);
		}
		else {
			glDisable(GL_DEPTH_TEST);
		}

		// Set tiling factor for the first detail texture if available
		if (material->hasParameter("TilingFactor1")) {
			float tilingFactor1 = material->getParameter("TilingFactor1");
			shader->setFloat("TilingFactor1", tilingFactor1);
		}

		// Set tiling factor for the first detail texture if available
		if (material->hasParameter("TilingFactor2")) {
			float tilingFactor2 = material->getParameter("TilingFactor2");
			shader->setFloat("TilingFactor2", tilingFactor2);
		}

	}

	// Pass the matrices to the shader.
	shader->setMat4("model", transform);

	if (animator) {
		auto transforms = animator->GetFinalBoneMatrices();
		auto numTransforms = transforms.size();

		//std::cout << "Number of transforms: " << numTransforms << std::endl;

		for (int i = 0; i < numTransforms; ++i) {
			//std::cout << "Setting finalBonesMatrices[" << i << "]" << std::endl;
			shader->setMat4x3("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
		}
	}
	else {
		std::cout << "Animator is null" << std::endl;
	}

	// Check for errors after setting uniforms
	while ((error = glGetError()) != GL_NO_ERROR) {
		std::cerr << "OpenGL Error after setting matrix uniforms: " << error << std::endl;
	}

	GLint maxTextureUnits;
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnits);

	// Use the Material::textureUniformMap to get the correct uniform names
	for (size_t i = 0; i < textures.size() && i < static_cast<size_t>(maxTextureUnits); ++i) {
		glActiveTexture(GL_TEXTURE0 + i);

		// Check if the texture is a cubemap
		if (textures[i].type == "environment") {
			glBindTexture(GL_TEXTURE_CUBE_MAP, textures[i].id);
			shader->setInt("environmentMap", i); // Set the cubemap uniform to the correct texture unit
		}
		else {
			glBindTexture(GL_TEXTURE_2D, textures[i].id);

			// Look up the uniform name from the map using the texture type
			auto uniformNameIt = Material::textureUniformMap.find(textures[i].type);
			if (uniformNameIt != Material::textureUniformMap.end()) {
				// Use the found uniform name to set the texture uniform in the shader
				shader->setInt(uniformNameIt->second, i);
				DEBUG_COUT << "Binding texture " << textures[i].path << " to " << uniformNameIt->second << std::endl;
			}
			else {
				std::cerr << "No uniform name found for texture type: " << textures[i].type << std::endl;
			}
		}
	}

	// Check for errors after texture binding
	while ((error = glGetError()) != GL_NO_ERROR) {
		std::cerr << "OpenGL Error after binding textures: " << error << std::endl;
	}

	DEBUG_COUT << "Drawing geometry with VAO ID: " << VAO << std::endl;
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0); // Reset active texture unit after binding

	// Check for errors after drawing
	while ((error = glGetError()) != GL_NO_ERROR) {
		std::cerr << "OpenGL Error after drawing: " << error << std::endl;
	}
}

void AnimatedGeometry::renderWireframeCube(const glm::mat4& transform, Animator* animator) {
	wireframeCubeShader.use();

	// Update the AABB based on the current animation pose
	//updateAABB(animator);

	// Calculate the scale and translation based on AABB dimensions
	glm::vec3 aabbCenter = (aabbMin + aabbMax) * 0.5f;
	glm::vec3 aabbExtents = (aabbMax - aabbMin);

	// Set the model matrix uniform in the shader
	glm::mat4 modelMatrix = glm::translate(transform, aabbCenter);
	modelMatrix = glm::scale(modelMatrix, aabbExtents);
	wireframeCubeShader.setMat4("model", modelMatrix);

	glBindVertexArray(wireframeCubeVAO);
	glDrawElements(GL_LINES, wireframeCubeIndices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void AnimatedGeometry::addTexture(const Texture& texture) {
	textures.push_back(texture);
}

void AnimatedGeometry::setMaterial(std::shared_ptr<Material> mat) {
	material = std::move(mat); // Assume ownership or shared reference of the passed material
	// Directly assign the shader std::shared_ptr from the material's shader program
	shader = material->getShaderProgram(); // This should return std::shared_ptr<Shader>
}

btCollisionShape* AnimatedGeometry::createBulletCollisionShape() const {
	auto mesh = new btTriangleMesh();

	for (size_t i = 0; i < indices.size(); i += 3) {
		int index0 = indices[i];
		int index1 = indices[i + 1];
		int index2 = indices[i + 2];

		const AnimatedVertex& v0 = vertices[index0];
		const AnimatedVertex& v1 = vertices[index1];
		const AnimatedVertex& v2 = vertices[index2];

		btVector3 vertex0(v0.Position.x, v0.Position.y, v0.Position.z);
		btVector3 vertex1(v1.Position.x, v1.Position.y, v1.Position.z);
		btVector3 vertex2(v2.Position.x, v2.Position.y, v2.Position.z);

		mesh->addTriangle(vertex0, vertex1, vertex2);
	}

	// Use the mesh to create a collision shape
	// Note: For static geometry, `useQuantizedAabbCompression = true` is recommended for performance
	auto shape = new btBvhTriangleMeshShape(mesh, true);
	return shape;
}

void AnimatedGeometry::addToPhysicsWorld(btDiscreteDynamicsWorld* dynamicsWorld) {
	btCollisionShape* shape = createBulletCollisionShape();

	// Obtain the model matrix that combines position, rotation, and scale
	glm::mat4 modelMatrix = getModelMatrix();

	// Convert glm::mat4 to btTransform
	btTransform transform;
	transform.setFromOpenGLMatrix(&modelMatrix[0][0]);

	// Use the transformed btDefaultMotionState for accurate positioning
	btDefaultMotionState* motionState = new btDefaultMotionState(transform);

	// Since the object is static (mass = 0), the local inertia is zero
	btVector3 localInertia(0, 0, 0);

	// Construct rigid body info using the mass, motion state, shape, and inertia
	btRigidBody::btRigidBodyConstructionInfo rbInfo(0, motionState, shape, localInertia);

	btRigidBody* body = new btRigidBody(rbInfo);

	// Optionally, store the body in a member variable if needed for later access or modification
	// rigidBody = std::unique_ptr<btRigidBody>(body);

	dynamicsWorld->addRigidBody(body);
}

void AnimatedGeometry::calculateAABB() {
	if (vertices.empty()) return;

	aabbMin = aabbMax = vertices[0].Position;

	// Determine min and max from vertex positions
	for (const auto& vertex : vertices) {
		//std::cout << "Vertex Position: x = " << vertex.Position.x << ", y = " << vertex.Position.y << ", z = " << vertex.Position.z << std::endl;
		aabbMin = glm::min(aabbMin, vertex.Position);
		aabbMax = glm::max(aabbMax, vertex.Position);
	}

	//std::cout << "Pre-Transform AABB Min: " << aabbMin.x << ", " << aabbMin.y << ", " << aabbMin.z << std::endl;
	//std::cout << "Pre-Transform AABB Max: " << aabbMax.x << ", " << aabbMax.y << ", " << aabbMax.z << std::endl;

	// Apply the current model matrix to the AABB corners
	glm::mat4 modelMatrix = getModelMatrix();
	glm::vec4 minCorner = modelMatrix * glm::vec4(aabbMin, 1.0f);
	glm::vec4 maxCorner = modelMatrix * glm::vec4(aabbMax, 1.0f);

	// Update the aabbMin and aabbMax with the new transformed corners
	aabbMin = glm::vec3(minCorner);
	aabbMax = glm::vec3(maxCorner);

	//std::cout << "Post-Transform AABB Min: " << aabbMin.x << ", " << aabbMin.y << ", " << aabbMin.z << std::endl;
	//std::cout << "Post-Transform AABB Max: " << aabbMax.x << ", " << aabbMax.y << ", " << aabbMax.z << std::endl;
}

void AnimatedGeometry::updateAABB(Animator* animator) {
	if (!animator) return;

	const auto& transforms = animator->GetFinalBoneMatrices();
	glm::vec3 localMin(std::numeric_limits<float>::max()), localMax(std::numeric_limits<float>::lowest());

	for (const auto& vertex : vertices) {
		glm::vec3 transformedVertex(0.0f);
		for (int i = 0; i < 4; ++i) {
			int boneID = vertex.BoneIDs[i];
			if (boneID >= 0 && boneID < transforms.size()) {
				transformedVertex += glm::vec3(transforms[boneID] * glm::vec4(vertex.Position, 1.0f)) * vertex.Weights[i];
			}
		}
		localMin = glm::min(localMin, transformedVertex);
		localMax = glm::max(localMax, transformedVertex);
	}

	aabbMin = localMin;
	aabbMax = localMax;
}


bool AnimatedGeometry::isInFrustum(const Frustum& frustum) const {
	for (int i = 0; i < 6; ++i) {
		const auto& plane = frustum.planes[i];

		glm::vec3 pVertex = aabbMin;
		if (plane.normal.x >= 0) pVertex.x = aabbMax.x;
		if (plane.normal.y >= 0) pVertex.y = aabbMax.y;
		if (plane.normal.z >= 0) pVertex.z = aabbMax.z;

		if (glm::dot(plane.normal, pVertex) + plane.distance < 0) {
			return false; // AABB is outside the frustum
		}
	}
	return true; // AABB is inside the frustum
}

glm::mat4 AnimatedGeometry::getModelMatrix() const {
	glm::mat4 model = glm::mat4(1.0f);
	// Apply scaling
	model = glm::scale(model, scale);
	// Apply rotation
	model = glm::rotate(model, glm::radians(rotationAngle), rotationAxis);
	// Apply translation
	model = glm::translate(model, position);
	return model;
}

void AnimatedGeometry::updateModelMatrix() {
	modelMatrix = glm::mat4(1.0f);
	// Apply scaling first
	modelMatrix = glm::scale(modelMatrix, scale);
	// Then apply rotation, making sure to convert the angle to radians
	modelMatrix = glm::rotate(modelMatrix, glm::radians(rotationAngle), rotationAxis);
	// Finally, apply translation
	modelMatrix = glm::translate(modelMatrix, position);
}

void AnimatedGeometry::setShader(std::shared_ptr<Shader> newShader) {
	shader = std::move(newShader); // Use std::move if you're transferring ownership
}

std::shared_ptr<Shader> AnimatedGeometry::getShader() const {
	return shader; // Directly return the std::shared_ptr<Shader>
}

void AnimatedGeometry::generateWireframeCubeVertices() {
	// Generate vertices for a unit cube
	wireframeCubeVertices = {
		glm::vec3(-0.5f, -0.5f, -0.5f),
		glm::vec3(0.5f, -0.5f, -0.5f),
		glm::vec3(0.5f, 0.5f, -0.5f),
		glm::vec3(-0.5f, 0.5f, -0.5f),
		glm::vec3(-0.5f, -0.5f, 0.5f),
		glm::vec3(0.5f, -0.5f, 0.5f),
		glm::vec3(0.5f, 0.5f, 0.5f),
		glm::vec3(-0.5f, 0.5f, 0.5f)
	};

	wireframeCubeIndices = {
		0, 1, 1, 2, 2, 3, 3, 0,
		4, 5, 5, 6, 6, 7, 7, 4,
		0, 4, 1, 5, 2, 6, 3, 7
	};
}

glm::vec3 AnimatedGeometry::getTranslation() const {
	return position;
}


