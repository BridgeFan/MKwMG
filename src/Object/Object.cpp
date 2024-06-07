//
// Created by kamil-hp on 20.03.2022.
//

#include "Object.h"
#include "Util.h"
#include "src/ImGui/ImGuiUtil.h"

const bf::Scene* bf::Object::scene = nullptr;
const bf::ConfigState* bf::Object::configState = nullptr;
int bf::Object::_objIndex = 1;

void bf::Object::setNewTransform(const glm::vec3& centre, const bf::Transform& oldTransform, const bf::Transform& newTransform) {
    if(oldTransform==newTransform)
        return;
    //TODO - non-uniform scaling in different ways
	glm::mat4 matrix = newTransform.CalculateMatrix()*oldTransform.CalculateInverseMatrix();
	auto vec = glm::vec4(getPosition()-centre,1.f);
	setPosition(glm::vec3(matrix*vec)+centre);
    transform.scale *= (newTransform.scale / oldTransform.scale);
    //rotation
    glm::vec3 r = getRotation();
    auto rotMat = bf::getRotateMatrix(newTransform.rotation)*bf::getInverseRotateMatrix(oldTransform.rotation)*bf::getRotateMatrix(r);
    r = bf::matrixToEulerXYZ(rotMat);
    setRotation(bf::degrees(r));
}

void bf::Object::ObjectGui() {
	bf::imgui::checkChanged("Object name", name);
    glm::vec3 pos = getPosition();
	if(bf::imgui::checkChanged("Object position", pos)) {
        setPosition(pos);
    }
    glm::vec3 rot = getRotation();
	if(bf::imgui::checkChanged("Object rotation", rot)) {
        setRotation(rot);
    }
    glm::vec3 s = getScale();
	if(bf::imgui::checkChanged("Object scale", s, true)) {
        setScale(s);
    }
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
glm::vec4 bf::Object::clampParam(float u, float v, float modulo) const {
	glm::vec4 ret;
	if (parameterWrappingU()) {
		ret.x = std::fmod(u, getParameterMax().x);
		if(ret.x<0) ret.x += getParameterMax().x;
	}
	else
		ret.x = std::max(std::min(u, getParameterMax().x), getParameterMin().x);
	if (parameterWrappingV()) {
		ret.y = std::fmod(v, getParameterMax().y);
		if(ret.y<0) ret.y += getParameterMax().y;
	}
	else
		ret.y = std::max(std::min(v, getParameterMax().y), getParameterMin().y);
	if(modulo>.0f) {
		ret.z = std::floor(ret.x / modulo);
		ret.w = std::floor(ret.y / modulo);
		ret.x = std::fmod(ret.x, modulo);
		ret.y = std::fmod(ret.y, modulo);
	}
	return ret;
}
glm::vec3 bf::Object::parameterFunction(float, float) const {return {0.f,0.f,0.f};}
glm::vec3 bf::Object::parameterGradientU(float, float) const {return {1.f,0.f,0.f};}
glm::vec3 bf::Object::parameterGradientV(float, float) const {return {0.f,1.f,0.f};}
