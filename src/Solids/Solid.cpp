//
// Created by kamil-hp on 16.03.2022.
//

#include "Solid.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "../Shader.h"
int bf::Solid::index = 1;

bf::Solid::~Solid() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &IBO);
}

void bf::Solid::setBuffers() {
	//remove old ones
	if(VAO<UINT_MAX)
		glDeleteVertexArrays(1, &VAO);
	if(VBO<UINT_MAX)
		glDeleteBuffers(1, &VBO);
	if(IBO<UINT_MAX)
		glDeleteBuffers(1, &IBO);
	//set buffers
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float) * 3, vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), reinterpret_cast<void*>(0));
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned), &indices[0], GL_STATIC_DRAW);
}

void bf::Solid::draw(const bf::Shader& shader) const {
    draw(shader, Transform::Default);
}

void bf::Solid::draw(const bf::Shader& shader, const bf::Transform& relativeTo) const {
    //function assumes set projection and view matrices
    glBindVertexArray(VAO);
    /*glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    model = glm::translate(model, {.0f,.0f,.0f});*/
    shader.setMat4("model", getModelMatrix(relativeTo));

    //glDrawArrays(GL_TRIANGLES, 0, vertices.size()/3);
    glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT,   // type
                   reinterpret_cast<void*>(0)           // element array buffer offset
    );
}

void bf::Solid::ObjectGui() {
	Object::ObjectGui();
}
