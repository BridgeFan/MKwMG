//
// Created by kamil-hp on 20.03.2022.
//

#include "Point.h"
#include <GL/glew.h>
#include "../Shader.h"
#include "../Object/ObjectArray.h"
int bf::Point::index = 1;
bool bf::Point::isInited = false;
unsigned bf::Point::VBO = UINT_MAX;
unsigned bf::Point::VAO = UINT_MAX;
bf::ObjectArray* bf::Point::objectArray = nullptr;

void bf::Point::draw(const Shader &shader) const {
	//function assumes set projection and view matrices
	glBindVertexArray(VAO);
	/*glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
	model = glm::translate(model, {.0f,.0f,.0f});*/
	shader.setMat4("model", getModelMatrix());

	//glDrawArrays(GL_TRIANGLES, 0, vertices.size()/3);
	glDrawArrays(GL_POINTS, 0, 1);
	/*glDrawElements(GL_POINTS, 1, GL_UNSIGNED_INT,   // type
	               reinterpret_cast<void*>(0)           // element array buffer offset
	);*/
}

void bf::Point::Init() {
	//remove old ones
	if(VAO<UINT_MAX)
		glDeleteVertexArrays(1, &VAO);
	if(VBO<UINT_MAX)
		glDeleteBuffers(1, &VBO);
	//set buffers
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	float vert[] = {.0f,.0f,.0f};

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3, vert, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), reinterpret_cast<void*>(0));
	glEnableVertexAttribArray(0);
}

void bf::Point::initObjArrayRef(bf::ObjectArray &objArray) {
	objectArray=&objArray;
}

bf::Point::Point(const bf::Transform &t, const std::string &pointName) : bf::Object(t, pointName) {
	if(!isInited) {
		Init();
		isInited=true;
	}
}

void notify(bf::ObjectArray* objectArray, bf::Point* tis) {
	if(objectArray) {
		unsigned i;
		for(i=0;i<objectArray->size();i++) {
			auto ptr = objectArray->getPtr(i);
			if (ptr == tis)
				break;
		}
		if(i<objectArray->size())
			objectArray->onMove(i);
	}
}

void bf::Point::setPosition(const glm::vec3 &pos) {
	Object::setPosition(pos);
	notify(objectArray, this);
}

void bf::Point::setTransform(const bf::Transform &t) {
	if(getTransform().position==t.position)
		return;
	Object::setTransform(t);
	notify(objectArray, this);
}

void bf::Point::setTransform(bf::Transform &&t) {
	if(getTransform().position==t.position)
		return;
	Object::setTransform(t);
	notify(objectArray, this);
}
