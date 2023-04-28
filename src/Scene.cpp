//
// Created by kamil-hp on 28.03.23.
//

#include <GL/glew.h>
#include "Scene.h"
#include "ConfigState.h"
#include "Shader.h"
#include "ShaderArray.h"
#include <glm/gtc/epsilon.hpp>

const glm::vec4 bf::Scene::clearColor = {0.25f, 0.25f, 0.20f, 1.00f};
const glm::vec4 clear_color = glm::vec4(0.25f, 0.25f, 0.20f, 1.00f);

void bf::Scene::draw(bf::ShaderArray &shaderArray, const ConfigState& configState) {
	glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shaderArray.changeShader(bf::ShaderType::BasicShader);
	// pass projection matrix to shader (note that in this case it could change every frame)
    float aspect = static_cast<float>(configState.screenWidth)/static_cast<float>(configState.screenHeight);
	projection = bf::getProjectionMatrix(configState.cameraZoom, aspect, camera.zNear, camera.zFar);
	inverseProjection = bf::getInverseProjectionMatrix(configState.cameraZoom, aspect, camera.zNear, camera.zFar);
	shaderArray.addCommonUniform("projection", projection);
	// camera/view transformation
	view = camera.GetViewMatrix();
	inverseView = camera.GetInverseViewMatrix(view);
	shaderArray.addCommonUniform("view", view);
	//draw objects
    std::vector<unsigned> indices;
	if(objectArray.isMultipleActive()) {
		multiCursor.transform.position+=objectArray.getCentre();
		multiCursor.draw(shaderArray, configState);
		multiCursor.transform.position-=objectArray.getCentre();
	}
	else if(objectArray.isMovable(objectArray.getActiveIndex())) {
		bf::Transform oldTransform = multiCursor.transform;
		multiCursor.transform=objectArray[objectArray.getActiveIndex()].getTransform();
		multiCursor.draw(shaderArray, configState);
		multiCursor.transform=std::move(oldTransform);
	}
	cursor.draw(shaderArray);
	objectArray.draw(shaderArray);
}

bf::Scene::Scene(float aspect, glm::vec3 &&cameraPos, glm::vec3 &&cameraRot, float cameraZoom, float cameraNear, float cameraFar) :
	objectArray(), cursor(), multiCursor(), camera(cameraNear,cameraFar, cameraPos,cameraRot) {
	projection = bf::getProjectionMatrix(cameraZoom,aspect, camera.zNear, camera.zFar);
	inverseProjection = bf::getInverseProjectionMatrix(cameraZoom,aspect, camera.zNear, camera.zFar);
	view = camera.GetViewMatrix();
	inverseView = camera.GetInverseViewMatrix(view);

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
