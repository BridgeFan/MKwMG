//
// Created by kamil-hp on 28.03.23.
//

#include <GL/glew.h>
#include "Scene.h"
#include "Settings.h"
#include "Shader.h"

const glm::vec4 bf::Scene::clearColor = {0.25f, 0.25f, 0.20f, 1.00f};
const glm::vec4 clear_color = glm::vec4(0.25f, 0.25f, 0.20f, 1.00f);

void bf::Scene::draw(bf::Shader &shader, const Settings& settings, int width, int height) {
	float aspect = static_cast<float>(width)/static_cast<float>(height);
	glViewport(0, 0, width, height);
	glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	shader.use();
	// pass projection matrix to shader (note that in this case it could change every frame)
	projection = bf::getProjectionMatrix(camera.Zoom,aspect, camera.zNear, camera.zFar);
	inverseProjection = bf::getInverseProjectionMatrix(camera.Zoom,aspect, camera.zNear, camera.zFar);
	shader.setMat4("projection", projection);
	// camera/view transformation
	view = camera.GetViewMatrix();
	inverseView = camera.GetInverseViewMatrix();
	shader.setMat4("view", view);
	//draw objects
	objectArray.draw(shader);
	if(settings.isMultiState && objectArray.isAnyActive()) {
		multiCursor.transform.position+=objectArray.getCentre();
		multiCursor.draw(shader, settings);
		multiCursor.transform.position-=objectArray.getCentre();
	}
	else if(!settings.isMultiState && objectArray.isMovable(objectArray.getActiveIndex())) {
		bf::Transform oldTransform = multiCursor.transform;
		multiCursor.transform=objectArray[objectArray.getActiveIndex()].getTransform();
		multiCursor.draw(shader, settings);
		multiCursor.transform=std::move(oldTransform);
	}
	cursor.draw(shader, settings);
}

bf::Scene::Scene(float aspect, glm::vec3 &&cameraPos, glm::vec3 &&cameraRot, float cameraNear, float cameraFar) :
	objectArray(), cursor(), multiCursor(), camera(cameraNear,cameraFar,cameraPos,cameraRot) {
	projection = bf::getProjectionMatrix(camera.Zoom,aspect, camera.zNear, camera.zFar);
	inverseProjection = bf::getInverseProjectionMatrix(camera.Zoom,aspect, camera.zNear, camera.zFar);
	view = camera.GetViewMatrix();
	inverseView = camera.GetInverseViewMatrix();

}

const glm::mat4 &bf::Scene::getProjection() const {
	return projection;
}

const glm::mat4 &bf::Scene::getInverseProjection() const {
	return inverseProjection;
}

const glm::mat4 &bf::Scene::getView() const {
	return view;
}

const glm::mat4 &bf::Scene::getInverseView() const {
	return inverseView;
}
