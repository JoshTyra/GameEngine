#include "LevelGeometry.h"
#include "shader.h"
#include "Materials.h"
#include "Debug.h"

LevelGeometry::LevelGeometry()
	: VAO(0), VBO(0), EBO(0), shader(nullptr) {
	// Initialize with empty data or default values
}

LevelGeometry::LevelGeometry(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<Texture>& textures)
	: vertices(vertices), indices(indices), textures(textures), VAO(0), VBO(0), EBO(0), shader(nullptr) {
	setupMesh();
	// Apply default transformations
	setPosition(glm::vec3(0.0f)); // Set the initial position if needed
	setScale(glm::vec3(0.025f)); // Scale the model uniformly
	setRotation(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate the model
}

void LevelGeometry::setupMesh() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	// Vertex normals (if needed in shader)
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

	// Vertex texture coords (first UV channel)
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

	// Assuming LightMapTexCoords is properly set in your Vertex struct
	glEnableVertexAttribArray(3); // Assuming location 3 for lightmap UVs
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, LightMapTexCoords));

	glBindVertexArray(0);
}

void LevelGeometry::Draw(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) {
	if (!shader) {
		std::cerr << "Shader not set for geometry, cannot draw." << std::endl;
		return;
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

	shader->use();
	shader->setMat4("model", model);
	shader->setMat4("view", view);
	shader->setMat4("projection", projection);

	// Use the Material::textureUniformMap to get the correct uniform names
	for (size_t i = 0; i < textures.size(); ++i) {
		glActiveTexture(GL_TEXTURE0 + i);
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

	DEBUG_COUT << "Drawing geometry with VAO ID: " << VAO << std::endl;
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0); // Reset active texture unit after binding
}

void LevelGeometry::addTexture(const Texture& texture) {
	textures.push_back(texture);
}

glm::mat4 LevelGeometry::getModelMatrix() const {
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, position); // Apply translation last
	model = glm::rotate(model, rotationAngle, rotationAxis); // Then apply rotation
	model = glm::scale(model, scale); // Apply scale first
	return model;
}

void LevelGeometry::setMaterial(std::shared_ptr<Material> mat) {
	material = mat;

	// Directly assign the shader pointer from the material's shader program
	shader = material->getShaderProgram();
}

btCollisionShape* LevelGeometry::createBulletCollisionShape() const {
	auto mesh = new btTriangleMesh();

	for (size_t i = 0; i < indices.size(); i += 3) {
		int index0 = indices[i];
		int index1 = indices[i + 1];
		int index2 = indices[i + 2];

		const Vertex& v0 = vertices[index0];
		const Vertex& v1 = vertices[index1];
		const Vertex& v2 = vertices[index2];

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

void LevelGeometry::addToPhysicsWorld(btDiscreteDynamicsWorld* dynamicsWorld) {
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






