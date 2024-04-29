#include "StaticGeometry.h"

StaticGeometry::StaticGeometry()
	: VAO(0), VBO(0), EBO(0), shader(nullptr), modelMatrix(glm::mat4(1.0f)),
	position(glm::vec3(0.0f)), rotationAxis(glm::vec3(0.0f, 0.0f, 0.0f)),
	rotationAngle(0.0f), scale(glm::vec3(0.025f)) {
}

// Overloaded constructor for initializing with mesh data
StaticGeometry::StaticGeometry(const std::vector<StaticVertex>& vertices,
	const std::vector<unsigned int>& indices,
	const std::vector<Texture>& textures)
	: vertices(vertices), indices(indices), textures(textures),
	VAO(0), VBO(0), EBO(0), shader(nullptr),
	position(glm::vec3(0.0f)), rotationAxis(glm::vec3(1.0f, 0.0f, 0.0f)),
	rotationAngle(-90.0f), scale(glm::vec3(0.025f)), modelMatrix(glm::mat4(1.0f))
{
	setupMesh();
	calculateAABB();
}

StaticGeometry::~StaticGeometry() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void StaticGeometry::setupMesh() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(StaticVertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(StaticVertex), (void*)0);

	// Vertex normals (if needed in shader)
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(StaticVertex), (void*)offsetof(StaticVertex, Normal));

	// Vertex texture coords (first UV channel)
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(StaticVertex), (void*)offsetof(StaticVertex, TexCoords));

	// Assuming LightMapTexCoords is properly set in your Vertex struct
	glEnableVertexAttribArray(3); // Assuming location 3 for lightmap UVs
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(StaticVertex), (void*)offsetof(StaticVertex, LightMapTexCoords));

	glBindVertexArray(0);
}

void StaticGeometry::draw() {
	if (!shader || !shader->Program) {
		std::cerr << "Shader not set or invalid for geometry, cannot draw." << std::endl;
		return;
	}

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

		// Set tiling factor for the second detail texture if available
		if (material->hasParameter("TilingFactor2")) {
			float tilingFactor2 = material->getParameter("TilingFactor2");
			shader->setFloat("TilingFactor2", tilingFactor2);
		}

		// Set material roughness parameter for materials that use this parameter
		if (material->hasParameter("roughness")) {
			float roughnessValue = material->getParameter("roughness");
			shader->setFloat("roughness", roughnessValue);
		}
	}

	// Update the model matrix based on current position, rotation, and scale.
	glm::mat4 model = getModelMatrix();

	// Pass the matrices to the shader.
	shader->setMat4("model", model);

	if (shader->hasUniform("normalMatrix")) {
		glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
		shader->setMat3("normalMatrix", normalMatrix);
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

void StaticGeometry::addTexture(const Texture& texture) {
	textures.push_back(texture);
}

void StaticGeometry::setMaterial(std::shared_ptr<Material> mat) {
	material = std::move(mat); // Assume ownership or shared reference of the passed material
	// Directly assign the shader std::shared_ptr from the material's shader program
	shader = material->getShaderProgram(); // This should return std::shared_ptr<Shader>
}

btCollisionShape* StaticGeometry::createBulletCollisionShape() const {
	auto mesh = new btTriangleMesh();

	for (size_t i = 0; i < indices.size(); i += 3) {
		int index0 = indices[i];
		int index1 = indices[i + 1];
		int index2 = indices[i + 2];

		const StaticVertex& v0 = vertices[index0];
		const StaticVertex& v1 = vertices[index1];
		const StaticVertex& v2 = vertices[index2];

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

void StaticGeometry::addToPhysicsWorld(btDiscreteDynamicsWorld* dynamicsWorld) {
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

void StaticGeometry::calculateAABB() {
	if (vertices.empty()) return;

	aabbMin = aabbMax = vertices[0].Position;

	for (const auto& vertex : vertices) {
		aabbMin = glm::min(aabbMin, vertex.Position);
		aabbMax = glm::max(aabbMax, vertex.Position);
	}

	// Optionally, apply the current model matrix to the AABB corners
	glm::mat4 modelMatrix = getModelMatrix();
	glm::vec4 minCorner = modelMatrix * glm::vec4(aabbMin, 1.0f);
	glm::vec4 maxCorner = modelMatrix * glm::vec4(aabbMax, 1.0f);

	aabbMin = glm::vec3(minCorner);
	aabbMax = glm::vec3(maxCorner);
}

bool StaticGeometry::isInFrustum(const Frustum& frustum) const {
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

glm::mat4 StaticGeometry::getModelMatrix() const {
	glm::mat4 model = glm::mat4(1.0f);
	// Apply scaling
	model = glm::scale(model, scale);
	// Apply rotation
	model = glm::rotate(model, glm::radians(rotationAngle), rotationAxis);
	// Apply translation
	model = glm::translate(model, position);
	return model;
}

void StaticGeometry::updateModelMatrix() {
	modelMatrix = glm::mat4(1.0f);
	// Apply scaling first
	modelMatrix = glm::scale(modelMatrix, scale);
	// Then apply rotation, making sure to convert the angle to radians
	modelMatrix = glm::rotate(modelMatrix, glm::radians(rotationAngle), rotationAxis);
	// Finally, apply translation
	modelMatrix = glm::translate(modelMatrix, position);
}

void StaticGeometry::setPosition(const glm::vec3& pos) {
	position = pos;
	updateModelMatrix();
}

void StaticGeometry::setRotation(float angle, const glm::vec3& axis) {
	rotationAngle = angle;
	rotationAxis = axis;
	updateModelMatrix();
}

void StaticGeometry::setScale(const glm::vec3& scl) {
	scale = scl;
	updateModelMatrix();
}

void StaticGeometry::setShader(std::shared_ptr<Shader> newShader) {
	shader = std::move(newShader); // Use std::move if you're transferring ownership
}

std::shared_ptr<Shader> StaticGeometry::getShader() const {
	return shader; // Directly return the std::shared_ptr<Shader>
}