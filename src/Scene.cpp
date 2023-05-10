//
// Created by kamil-hp on 28.03.23.
//

#include <GL/glew.h>
#include <array>
#include "Scene.h"
#include "ConfigState.h"
#include "src/Shader/Shader.h"
#include "src/Shader/ShaderArray.h"
#include "src/Object/Point.h"
#include "Solids/Torus.h"
#include "Event.h"
#include "Util.h"
#include "FileLoading.h"

void bf::Scene::internalDraw(const ConfigState& configState) {
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
	// camera/view transformation
	view = camera.GetViewMatrix();
	inverseView = camera.GetInverseViewMatrix(view);
	shaderArray.addCommonUniform("view", view);
	projection = bf::getProjectionMatrix(configState.cameraFOV, aspect,
			configState.cameraNear, configState.cameraFar);
	inverseProjection = bf::getInverseProjectionMatrix(configState.cameraFOV, aspect,
			configState.cameraNear, configState.cameraFar);
	if(!configState.stereoscopic) {
		shaderArray.addCommonUniform("projection", projection);
		shaderArray.setStereoscopicState(bf::StereoscopicState::None);
		internalDraw(configState);
	}
	else {
		//TODO - change GPU projection matrix
		shaderArray.setStereoscopicState(bf::StereoscopicState::LeftEye);
		internalDraw(configState);
		//TODO - change GPU projection matrix
		shaderArray.setStereoscopicState(bf::StereoscopicState::RightEye);
		internalDraw(configState);
	}
}

bf::Scene::Scene(const ConfigState& configState) :
	objectArray(), cursor(), multiCursor(), camera(configState.getCameraInitPos(),configState.getCameraInitRot()) {
    float aspect = static_cast<float>(configState.screenWidth)/static_cast<float>(configState.screenHeight);
	projection = bf::getProjectionMatrix(configState.cameraFOV, aspect,
                                         configState.cameraNear, configState.cameraFar);
	inverseProjection = bf::getInverseProjectionMatrix(configState.cameraFOV, aspect,
        configState.cameraNear, configState.cameraFar);
	view = camera.GetViewMatrix();
	inverseView = camera.GetInverseViewMatrix(view);
    shaderArray.setGrayPercentage(configState.grayPercentage);
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
    if(objectArray.onKeyReleased(key, modKeyBit))
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

glm::vec3 blockAxes(const glm::vec3& v, uint8_t axisLock) {
	glm::vec3 a = v;
	if(axisLock&0x01)
		a.x=0.f;
	if(axisLock&0x02)
		a.y=0.f;
	if(axisLock&0x04)
		a.z=0.f;
	return a;
}

void deAlmostZero(glm::vec3& v, const glm::vec3& diffV) {
	std::array<bool,3> sgn = {std::signbit(v.x),std::signbit(v.y),std::signbit(v.z)};
	v += diffV;
	if(std::abs(v.x)<0.001f)
		v.x = sgn[0] ? -0.001f : 0.001f;
	if(std::abs(v.y)<0.001f)
		v.y = sgn[1] ? -0.001f : 0.001f;
	if(std::abs(v.z)<0.001f)
		v.z = sgn[2] ? -0.001f : 0.001f;
}

void bf::Scene::onMouseMove(const glm::vec2 &oldMousePos, const bf::ConfigState &configState) {
	objectArray.onMouseMove(oldMousePos,{configState.mouseX,configState.mouseY});
	//other operations
	float speed = configState.movementSpeed * configState.deltaTime;
	float rotSpeed = configState.rotationSpeed * configState.deltaTime;
	float scaleSpeed = .5f * configState.deltaTime;
	if(configState.state != bf::None) {
		float myVec[] = {.0f,.0f,.0f};
		if(configState.isAltPressed)
			myVec[2] = configState.mouseX - oldMousePos.x;
		else
		{
			myVec[0] = configState.mouseX - oldMousePos.x;
			myVec[1] = oldMousePos.y - configState.mouseY;
		}
		auto glmVec = glm::vec3(myVec[0],myVec[1],myVec[2]);
		auto rotatedGlmVec = bf::rotate(glmVec, camera.rotation);
		auto blockedPosVec = blockAxes(rotatedGlmVec, configState.isAxesLocked);
		auto blockedRotVec = blockAxes({myVec[1], myVec[0], myVec[2]}, configState.isAxesLocked); //swapped X and Y
		if (configState.isCtrlPressed) {
			//camera movement
			if(configState.state == bf::MiddleClick) {
				camera.position += bf::rotate(speed * glmVec, camera.rotation);
			}
			else if(configState.state == bf::RightClick) {
				std::swap(glmVec[0],glmVec[1]);
				glm::vec3 centre;
				if(objectArray.isMultipleActive())
					centre = multiCursor.transform.position;
				else if(objectArray.isMovable(objectArray.getActiveIndex()))
					centre = objectArray[objectArray.getActiveIndex()].getPosition();
				else
					centre = cursor.transform.position;
				bf::Transform rotated = rotateAboutPoint(camera, centre, rotSpeed * glmVec);
				camera.position = rotated.position;
				camera.rotation = rotated.rotation;
			}
		}
		else {
			bf::Transform deltaTransform;
			deltaTransform.scale = glm::vec3(.0f);
			if (configState.state != bf::None && configState.isShiftPressed) {
				if(configState.isUniformScaling) {
					float vecValue = scaleSpeed * ((glmVec[0]+glmVec[1])*.5f+glmVec[2]);
					deAlmostZero(deltaTransform.scale,  {vecValue ,vecValue, vecValue});
				}
				else {
					deAlmostZero(deltaTransform.scale, scaleSpeed * blockedPosVec);
				}
			} else if (configState.state == bf::MiddleClick) {
				deltaTransform.position += speed * blockedPosVec;
			} else if (configState.state == bf::RightClick) {
				deltaTransform.rotation += rotSpeed * blockedRotVec;
			}
			//object movement
			bf::Transform t;
			if(objectArray.isMultipleActive())
				t = multiCursor.transform;
			else if(objectArray.isMovable(objectArray.getActiveIndex()))
				t = objectArray[objectArray.getActiveIndex()].getTransform();
			else
				t = cursor.transform;
			t.position += deltaTransform.position;
			t.rotation += deltaTransform.rotation;
			t.scale += deltaTransform.scale;
			if(objectArray.isMultipleActive()) {
				for (std::size_t i = 0; i < objectArray.size(); i++) {
					if (objectArray.isCorrect(i) && objectArray.isActive(i)) {
						objectArray[i].setNewTransform(objectArray.getCentre(), multiCursor.transform, t);
					}
				}
				multiCursor.transform = std::move(t);
			}
			else if(objectArray.isMovable(objectArray.getActiveIndex())) {
				objectArray[objectArray.getActiveIndex()].setTransform(std::move(t));
			}
			else {
				glm::vec3 screenPos = bf::toScreenPos(configState.screenWidth,configState.screenHeight,
													  cursor.transform.position,getView(),getProjection());
				screenPos.x = configState.mouseX;
				screenPos.y = configState.mouseY;
				cursor.transform.position = bf::toGlobalPos(configState.screenWidth,
																  configState.screenHeight,screenPos, getInverseView(), getInverseProjection());
			}
		}
	}
}
