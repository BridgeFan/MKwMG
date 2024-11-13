//
// Created by kamil-hp on 15.03.2022.
//

#include "Camera.h"
#include "ConfigState.h"
#include "src/ImGui/ImGuiUtil.h"

void bf::Camera::ObjectGui(bf::ConfigState& configState) {
	bf::imgui::checkChanged("Position", position);
	bf::imgui::checkChanged("Rotation", rotation);
	bf::imgui::checkChanged("fov", configState.cameraFOV, 5.f, 120.f);
	bf::imgui::checkChanged("near", configState.cameraNear, .01f, 5.f, .05f, .5f);
	bf::imgui::checkChanged("far", configState.cameraFar, 5.f, 10000.f);
}

const glm::vec3 &bf::Camera::getFront() const {
	return front;
}

const glm::vec3 &bf::Camera::getUp() const {
	return up;
}

const glm::vec3 &bf::Camera::getRight() const {
	return right;
}

glm::mat4 bf::Camera::GetViewMatrix() {
	//update vectors
	right = rotate({1.0f, 0.0f, 0.0f}, rotation);
	up = rotate({0.0f, 1.0f, 0.0f}, rotation);
	front = rotate({0.0f, 0.0f, 1.0f}, rotation);
	//return matrix
	auto m3 = glm::mat3(1.f,0.f,0.f,
		0.f,1.f,0.f,
		0.f,0.f,1.f);
	glm::mat3 matrix = m3*glm::mat3(bf::getInverseRotateMatrix(rotation));
	auto ret = glm::mat4{{matrix[0], 0},
		{matrix[1], 0},
		{matrix[2], 0},
		{matrix * position, 1},
	};
	return ret;
}

glm::mat4 bf::Camera::GetInverseViewMatrix(const glm::mat4& view) {
	auto tmp = glm::inverse(view);
	//TODO - improve position
	auto m3 = glm::mat3(1.f,0.f,0.f,
		0.f,1.f,0.f,
		0.f,0.f,1.f);
	const glm::mat3 invMatrix = glm::mat3(bf::getRotateMatrix(rotation))*m3;
	auto ret = glm::mat4(invMatrix);
	ret[3] = tmp[3];
	/*glm::vec3 v = bf::rotate(bf::rotate(position,-rotation),rotation);
	ret[3] = glm::vec4(v, 1.f);
	for(int i=0;i<4;i++) {
		if(!almostEqual(ret[3][i],tmp[3][i],1e-4f)) {
			std::cout << ret[3][i] << " " << tmp[3][i] << "\n";
		}
	}*/
	return ret;
}

bf::Camera::Camera(glm::vec3 pos, glm::vec3 rot)
		: Transform(pos, rot) {}
