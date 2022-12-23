#include "Render.h"

RenderObject::RenderObject(std::vector<Vertex> &vertices_, std::vector<unsigned int>& indices_) :
vertices(vertices_),
indices(indices_)
{
	setupMesh();
}


RenderObject::~RenderObject() {
	glDeleteBuffers(1, & EBO);
	glDeleteBuffers(1, & VBO);
	glDeleteVertexArrays(1, & VAO);
}

void RenderObject::Draw(Shader* shader) {
	//shader.use();
	shader->use();

	// draw mesh
	glBindVertexArray(VAO);
	
	//glPolygonMode( GL_FRONT_AND_BACK, GL_LINE ); 
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL); 
	glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// always good practice to set everything back to defaults once configured.
	//glActiveTexture(GL_TEXTURE0);
}


void RenderObject::setupMesh() {
	// create buffers/arrays
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	// load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// A great thing about structs is that their memory layout is sequential for all its items.
	// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
	// again translates to 3/2 floats which translates to a byte array.
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// set the vertex attribute pointers
	// vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// vertex Colors 
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	// vertex Normals
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

	glBindVertexArray(0);
}



Model::Model(std::shared_ptr<RenderObject> ro_,
	std::shared_ptr<Shader> shader_) :
ro(ro_),
shader(shader_)
{
	world_transform = kIdentity44f;
}


void Model::Draw() {
	shader->use();

	glActiveTexture(GL_TEXTURE0);
	shader->setInt("texture_diffuse", 0);
	glBindTexture(GL_TEXTURE_2D, texture);


	shader->setMat4("model", world_transform);
	shader->setVec3("material.ambient", material->ambient);
	shader->setVec3("material.diffuse", material->diffuse);
	shader->setVec3("material.specular", material->specular);
	shader->setFloat("material.shininess", material->shininess);
	shader->setVec3("material.emission", material->emission);
	ro->Draw(shader.get());

	glActiveTexture(GL_TEXTURE0);
}
