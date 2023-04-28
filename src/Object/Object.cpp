//
// Created by kamil-hp on 20.03.2022.
//

#include <glm/gtx/euler_angles.hpp>
#include "Object.h"
#include "src/ImGui/ImGuiUtil.h"

const bf::Scene* bf::Object::scene = nullptr;
const bf::ConfigState* bf::Object::configState = nullptr;
int bf::Object::_objIndex = 1;

void bf::Object::setNewTransform(const glm::vec3& centre, const bf::Transform& oldTransform, const bf::Transform& newTransform) {
    if(oldTransform==newTransform)
        return;
    //TODO - non-uniform scaling in different ways
	glm::mat4 matrix = newTransform.CalculateMatrix()*oldTransform.CalculateInverseMatrix();
	auto vec = glm::vec4(transform.position-centre,1.f);
	setPosition(glm::vec3(matrix*vec)+centre);
    transform.scale *= (newTransform.scale / oldTransform.scale);
    //rotation
    auto rotMat = bf::getRotateMatrix(newTransform.rotation)*bf::getInverseRotateMatrix(oldTransform.rotation)*bf::getRotateMatrix(transform.rotation);
    glm::extractEulerAngleXYZ(rotMat, transform.rotation.x, transform.rotation.y, transform.rotation.z);
    setRotation(glm::degrees(transform.rotation));
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
	setPosition(transform.position + (pos-transform.position) * (multiplier-1));
}

glm::mat4 bf::Object::getModelMatrix(const bf::Transform &relativeTo) const {
	return transform.CalculateMatrix(relativeTo);
}

bool bf::Object::addPoint(unsigned int) { return false; }
bool bf::Object::onKeyPressed(bf::event::Key, bf::event::ModifierKeyBit) {return false;}
bool bf::Object::onKeyReleased(bf::event::Key, bf::event::ModifierKeyBit) {return false;}
bool bf::Object::onMouseButtonPressed(bf::event::MouseButton, bf::event::ModifierKeyBit) {return false;}
bool bf::Object::onMouseButtonReleased(bf::event::MouseButton, bf::event::ModifierKeyBit) {return false;}
void bf::Object::onMouseMove(const glm::vec2&, const glm::vec2&) {}

void bf::Object::initData(const bf::ConfigState &cs, const bf::Scene &s) {
    configState = &cs;
    scene = &s;
}
