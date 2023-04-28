//
// Created by kamil-hp on 28.03.23.
//

#include <GL/glew.h>
#include "Scene.h"
#include "ConfigState.h"
#include "src/Shader/Shader.h"
#include "src/Shader/ShaderArray.h"
#include "src/Object/Point.h"
#include "Solids/Torus.h"
#include "Event.h"
#include "Util.h"
#include "FileLoading.h"

void bf::Scene::draw(const ConfigState& configState) {
	float clearColorR = static_cast<float>(configState.backgroundColorR)/255.f;
	float clearColorG = static_cast<float>(configState.backgroundColorG)/255.f;
	float clearColorB = static_cast<float>(configState.backgroundColorB)/255.f;
	float clearColorA = static_cast<float>(255u)/255.f;
	glClearColor(clearColorR * clearColorA, clearColorG * clearColorA, clearColorB * clearColorA, clearColorA);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shaderArray.changeShader(bf::ShaderType::BasicShader);
	// pass projection matrix to shader (note that in this case it could change every frame)
    float aspect = static_cast<float>(configState.screenWidth)/static_cast<float>(configState.screenHeight);
	projection = bf::getProjectionMatrix(configState.cameraFOV, aspect, camera.zNear, camera.zFar);
	inverseProjection = bf::getInverseProjectionMatrix(configState.cameraFOV, aspect, camera.zNear, camera.zFar);
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
	objectArray.draw(shaderArray,configState);
}

bf::Scene::Scene(const ConfigState& configState) :
	objectArray(), cursor(), multiCursor(), camera(configState.getCameraNear(),configState.getCameraFar(),
		configState.getCameraInitPos(),configState.getCameraInitRot()) {
    float aspect = static_cast<float>(configState.screenWidth)/static_cast<float>(configState.screenHeight);
	projection = bf::getProjectionMatrix(configState.cameraFOV, aspect, camera.zNear, camera.zFar);
	inverseProjection = bf::getInverseProjectionMatrix(configState.cameraFOV, aspect, camera.zNear, camera.zFar);
	view = camera.GetViewMatrix();
	inverseView = camera.GetInverseViewMatrix(view);
    bf::Point::initObjArrayRef(objectArray);
    bf::Object::initData(configState, *this);
    if(!bf::loadFromFile(objectArray)) {
        objectArray.add<bf::Torus>(); //initial
    }
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

bool bf::Scene::onKeyPressed(bf::event::Key key, bf::event::ModifierKeyBit modKeyBit, const bf::ConfigState&) {
    using namespace bf::event;
    if(objectArray.onKeyPressed(key, modKeyBit))
        return true;
    switch(key) {
        case Key::C:
            objectArray.clearSelection();
            return true;
        case Key::P:
            objectArray.add<bf::Point>(cursor.transform);
            return true;
        case Key::T:
            objectArray.add<bf::Torus>(cursor.transform);
            return true;
        case Key::Delete:
            objectArray.removeActive();
            return true;
        default:
            return false;
    }
}

bool bf::Scene::onKeyReleased(bf::event::Key key, bf::event::ModifierKeyBit modKeyBit, const bf::ConfigState&) {
    if(objectArray.onKeyPressed(key, modKeyBit))
        return true;
    return false;
}

bool bf::Scene::onMouseButtonPressed(bf::event::MouseButton button, bf::event::ModifierKeyBit modKeyBit, const bf::ConfigState& configState) {
    using namespace bf::event;
    if(objectArray.onMouseButtonPressed(button, modKeyBit))
        return true;
    if(button==MouseButton::Left) {
        auto mouseXF = configState.mouseX;
        auto mouseYF = configState.mouseY;
        float sqrDist = configState.pointRadius*configState.pointRadius;
        int selectionIndex = -1;
        float actualZ = 9.999f;
        for(unsigned i=0u;i<objectArray.size();i++) {
            if(!objectArray.isCorrect(i))
                continue;
            auto screenPos = bf::toScreenPos(configState.screenWidth,configState.screenHeight,
                                             objectArray[i].getTransform().position, getView(), getProjection());
            if(screenPos==bf::outOfWindow)
                continue;
            float d = (screenPos.x-mouseXF)*(screenPos.x-mouseXF)+(screenPos.y-mouseYF)*(screenPos.y-mouseYF);
            if(d<=sqrDist && actualZ>screenPos.z) {
                selectionIndex=static_cast<int>(i);
                actualZ=screenPos.z;
            }
        }
        if(selectionIndex >= 0) {
            if(!configState.isCtrlPressed)
                objectArray.clearSelection(selectionIndex);
            else
                objectArray.toggleActive(selectionIndex);
            multiCursor.transform = bf::Transform();
        }
    }
    return false;
}

bool bf::Scene::onMouseButtonReleased(bf::event::MouseButton button, bf::event::ModifierKeyBit modKeyBit, const bf::ConfigState&) {
    if(objectArray.onMouseButtonReleased(button, modKeyBit))
        return true;
    return false;
}