//
// Created by kamil-hp on 20.03.2022.
//

#include <glm/gtx/euler_angles.hpp>
#include "Object.h"
#include "src/Settings.h"
#include "src/ImGuiUtil.h"

int bf::Object::_objIndex = 1;

void bf::Object::setNewTransform(const glm::vec3& centre, const bf::Transform& oldTransform, const bf::Transform& newTransform) {
    if(oldTransform==newTransform)
        return;
    //position
    auto v = glm::vec4(transform.position-(centre+oldTransform.position),1.f);
    glm::mat4 invMatrix = bf::getScalingMatrix(1.f/oldTransform.scale)*bf::getInverseRotateMatrix(oldTransform.rotation);
    glm::mat4 matrix = bf::getRotateMatrix(newTransform.rotation)*bf::getScalingMatrix(newTransform.scale);
    transform.position = glm::vec3(matrix * invMatrix * v) + centre + newTransform.position;
    //scale (assumed that scaling is INDEPENDENT of rotation)
    transform.scale *= (newTransform.scale / oldTransform.scale);
    //rotation
    auto rotMat = bf::getInverseRotateMatrix(oldTransform.rotation)*bf::getRotateMatrix(transform.rotation)*bf::getRotateMatrix(newTransform.rotation);
    glm::extractEulerAngleXYZ(rotMat, transform.rotation.x, transform.rotation.y, transform.rotation.z);
    transform.rotation = glm::degrees(transform.rotation);
}

void bf::Object::ObjectGui() {
	bf::imgui::checkChanged("Object name", name);
	bf::imgui::checkChanged("Object position", transform.position);
	bf::imgui::checkChanged("Object rotation", transform.rotation);
	bf::imgui::checkChanged("Object scale", transform.scale, true);
}

glm::vec3 bf::getMiddle(const std::vector<bf::Object>& objects) {
	glm::vec3 sumPos;
	for(const auto& o: objects) {
		sumPos+=o.getPosition();
	}
	return sumPos/=objects.size();
}

void bf::Object::setRelativeScale(const glm::vec3 &pos, float multiplier) {
	transform.position += (pos-transform.position) * (multiplier-1);
}

glm::mat4 bf::Object::getModelMatrix(const bf::Transform &relativeTo) const {
	return transform.CalculateMatrix(relativeTo);
}

bool bf::Object::addPoint(unsigned int) { return false; }
bool bf::Object::onKeyPressed(int, int) {return false;}
bool bf::Object::onKeyReleased(int, int) {return false;}
bool bf::Object::onMouseButtonPressed(int, int) {return false;}
bool bf::Object::onMouseButtonReleased(int, int) {return false;}
void bf::Object::onMouseMove(const glm::vec2&, const glm::vec2&) {}
