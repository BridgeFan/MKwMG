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
	//bezierDraw objects
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
    //draw box select
    if(configState.isBoxSelect) {
        shaderArray.changeShader(BasicShader);
        shaderArray.getActiveShader().setMat4("view", glm::mat4(1.f));
        shaderArray.getActiveShader().setMat4("projection", glm::mat4(1.f));
        shaderArray.setColor(255u, 255u, 0u);
        boxRectangle.draw(shaderArray);
        shaderArray.changeShader(BasicShader);
    }
}

void bf::Scene::draw(const ConfigState& configState) {
    //box select update
    static glm::vec2 oldMousePos = {configState.mouseX, configState.mouseY};
    static bool wasBoxSelect = configState.isBoxSelect;
    if(configState.isBoxSelect) {
        if(!wasBoxSelect || !almostEqual(oldMousePos.x,configState.mouseX) || !almostEqual(oldMousePos.y,configState.mouseY)) {
            float bx = 2.f * configState.boxMouseX / configState.screenWidth - 1.f;
            float by = -2.f * configState.boxMouseY / configState.screenHeight + 1.f;
            float mx = 2.f * configState.mouseX / configState.screenWidth - 1.f;
            float my = -2.f * configState.mouseY / configState.screenHeight + 1.f;
            boxRectangle.vertices[0].setPosition({bx, by, 0.f});
            boxRectangle.vertices[1].setPosition({mx, by, 0.f});
            boxRectangle.vertices[2].setPosition({mx, my, 0.f});
            boxRectangle.vertices[3].setPosition({bx, my, 0.f});
            boxRectangle.glUpdateVertices();
        }
    }
    oldMousePos = {configState.mouseX, configState.mouseY};
    wasBoxSelect = configState.isBoxSelect;
    //prepare color for draw
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
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
		shaderArray.addCommonUniform("projection", projection);
		shaderArray.setStereoscopicState(bf::StereoscopicState::None);
		internalDraw(configState);
	}
	else {
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glClearColor(clearColorR * clearColorA, clearColorG * clearColorA, clearColorB * clearColorA, clearColorA);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //TODO - change GPU projection matrix
		shaderArray.setStereoscopicState(bf::StereoscopicState::LeftEye);
        camera.position-=camera.getRight()*configState.IOD*.5f;
        shaderArray.addCommonUniform("view", camera.GetViewMatrix());
        shaderArray.addCommonUniform("projection", bf::getLeftProjectionMatrix(configState.cameraFOV,
               aspect, configState.cameraNear, configState.cameraFar, configState.convergence, configState.IOD));
        internalDraw(configState);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO+1);
        glClearColor(clearColorR * clearColorA, clearColorG * clearColorA, clearColorB * clearColorA, clearColorA);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//TODO - change GPU projection matrix
		shaderArray.setStereoscopicState(bf::StereoscopicState::RightEye);
        camera.position+=camera.getRight()*configState.IOD;
        shaderArray.addCommonUniform("view", camera.GetViewMatrix());
        shaderArray.addCommonUniform("projection", bf::getRightProjectionMatrix(configState.cameraFOV,
            aspect, configState.cameraNear, configState.cameraFar, configState.convergence, configState.IOD));
        internalDraw(configState);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(clearColorR * clearColorA, clearColorG * clearColorA, clearColorB * clearColorA, clearColorA);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        camera.position-=camera.getRight()*configState.IOD*.5f;
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture+1);
        shaderArray.changeShader(LinkShader);
        shaderArray.getActiveShader().setInt("texture1", 0);
        shaderArray.getActiveShader().setInt("texture2", 1);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, reinterpret_cast<void*>(0));
	}
}

bf::Scene::Scene(const ConfigState& configState) :
	objectArray(), cursor(), multiCursor(), camera(configState.getCameraInitPos(),configState.getCameraInitRot()),
    boxRectangle("",true) {
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
    //add framebuffers
    glGenFramebuffers(2, &FBO);
    glGenTextures(2, &texture);
    glGenRenderbuffers(2, &RBO);
    for(int i=0;i<2;i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, FBO+i);
        glEnable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBindTexture(GL_TEXTURE_2D, texture+i);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, configState.screenWidth, configState.screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture+i, 0);
        glBindRenderbuffer(GL_RENDERBUFFER, RBO+i);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, configState.screenWidth, configState.screenHeight); // use a single renderbuffer object for both a depth AND stencil buffer.
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO+i);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    const float vertices[] = {
            // positions        // texture coords
            1.f, 1.f, 0.0f,   1.0f, 1.0f,   // top right
            1.f, -1.f, 0.0f,   1.0f, 0.0f,   // bottom right
            -1.f, -1.f, 0.0f,0.0f, 0.0f,   // bottom left
            -1.f,  1.f, 0.0f,0.0f, 1.0f    // top left
    };
    const unsigned int indices[] = {
            0, 1, 3, // first triangle
            1, 2, 3  // second triangle
    };
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //set box drawing
    boxRectangle.vertices.resize(4, {glm::vec3(.0f)});
    boxRectangle.indices = {0, 1, 1, 2, 2, 3, 3, 0};
    boxRectangle.setBuffers();

    //load initial configuration
    if(!bf::loadFromFile(objectArray)) {
        objectArray.add<bf::Torus>();
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
	objectArray.onMouseMove(oldMousePos,{configState.mouseX,configState.mouseY}, configState, view, projection);
    if(configState.isBoxSelect) {
        return;
    }
	//other operations
	float speed = configState.movementSpeed * configState.deltaTime;
	float rotSpeed = configState.rotationSpeed * configState.deltaTime;
	float scaleSpeed = .5f * configState.deltaTime;
	if(configState.state != bf::None && !configState.isBoxSelect) {
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

bf::Scene::~Scene() {
    if(FBO<UINT_MAX) glDeleteFramebuffers(2, &FBO);
    if(texture<UINT_MAX) glDeleteTextures(2, &texture);
    if(RBO<UINT_MAX) glDeleteRenderbuffers(2, &RBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void bf::Scene::resizeFramebuffers(int x, int y) const {
    for(int i=0;i<2;i++) {
        glBindTexture(GL_TEXTURE_2D, texture+i);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glNamedRenderbufferStorage(RBO+i, GL_DEPTH_COMPONENT, x, y);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
}
