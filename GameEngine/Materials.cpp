#include <GL/glew.h>
#include "Materials.h"
#include "shader.h"

void Material::SetupRenderState() {

	// Bind diffuse texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseTexture);

	// Bind specular texture 
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, specularTexture);

	// Set shader values
	shader->setFloat("material.shininess", shininess);
}

void Material::ResetRenderState() {

	// Unbind any textures
	glBindTexture(GL_TEXTURE_2D, 0);

	// Reset other state...
}