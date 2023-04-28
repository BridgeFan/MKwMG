//
// Created by kamil-hp on 13.03.2022.
//
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Window.h"
#include "ConfigState.h"
#include "Camera.h"
#include <array>
#include "imgui-master/imgui.h"
#include "Util.h"
#include "src/Object/Cursor.h"
#include "Scene.h"
#ifdef USE_STD_FORMAT
#include <format>
#endif
#include <iostream>
#include "Event.h"
#include "ImGui/ImGuiUtil.h"
#include "ImGui/ImGuiPanel.h"

void GLAPIENTRY
MessageCallback( GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei, const GLchar* message, const void* ) {
    std::string sourceStr;
    switch (source) {
        case GL_DEBUG_SOURCE_API:				sourceStr="API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:		sourceStr="Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:	sourceStr="Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:		sourceStr="Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION:		sourceStr="Application"; break;
        case GL_DEBUG_SOURCE_OTHER: default:	sourceStr="Other"; break;
    }
    std::string typeStr;
    switch (type) {
        case GL_DEBUG_TYPE_ERROR:				typeStr="Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:	typeStr="Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:	typeStr="Undefined Behaviour"; break;
        case GL_DEBUG_TYPE_PORTABILITY:			typeStr="Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:			typeStr="Performance"; break;
        case GL_DEBUG_TYPE_MARKER:				typeStr="Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP:			typeStr="Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP:			typeStr="Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER: default:		typeStr="Other"; break;
    }
    std::string severityStr;
    switch(severity) {
        case GL_DEBUG_SEVERITY_HIGH:		severityStr="HIGH"; break;
        case GL_DEBUG_SEVERITY_MEDIUM:		severityStr="MEDIUM"; break;
        case GL_DEBUG_SEVERITY_LOW:			severityStr="LOW"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:return; //should not be shown
        default:							severityStr="?????";
    }
#ifdef USE_STD_FORMAT
    std::cerr << std::format("GL CALLBACK: id={}, source = {}, type = {}, severity = {}, message = {}\n",
                             id, sourceStr, typeStr, severityStr, message );
#else
    std::cerr << "GL CALLBACK: id=" << id << ", source = " << sourceStr << ", type = " << typeStr << ", severity = " <<
        severityStr << ", message = " << message << "\n";
#endif
}

//declaration of functions
GLFWwindow* initWindow(const bf::ConfigState& configState);
void framebuffer_size_callback(GLFWwindow*, int width, int height);
void mouse_callback(GLFWwindow*, double xposIn, double yposIn);
void scroll_callback(GLFWwindow*, double /*xoffset*/, double yoffset);
void key_callback(GLFWwindow*, int key, int /*scancode*/, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

bf::GlfwStruct::GlfwStruct(bf::ConfigState &configState1, GLFWwindow* window) : configState(configState1), io(window), scene(configState1) {}

bf::Window::~Window() {
	glfwDestroyWindow(window);
	glfwTerminate();
    delete glfwStruct;
}

bf::Window::Window(const bf::ConfigState &configState) {
    glfwStruct = nullptr;
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if defined(__APPLE__) || defined (__MACH__)
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    window=initWindow(configState);
    glfwSwapInterval(1);
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback,nullptr);
}

void bf::Window::run(bf::ConfigState &configState) {
	glfwStruct = new bf::GlfwStruct(configState, window);
	glfwSetWindowUserPointer(window,glfwStruct);
	if(!glfwStruct)
		return;
	auto& scene = glfwStruct->scene;
	while (!glfwWindowShouldClose(window))
	{
		configState.deltaTime = bf::getDeltaTime();
		///IMGUI
		bf::imgui::preDraw();
		bf::imgui::createObjectPanel(scene);
		bf::imgui::listOfObjectsPanel(scene,configState);
		bf::imgui::modifyObjectPanel(scene,configState);
		bf::imgui::cameraInfoPanel(scene, configState);
		///RENDER
		scene.draw(configState);
		bf::imgui::postDraw();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

//internal functions

GLFWwindow* initWindow(const bf::ConfigState& configState)
{
	GLFWwindow* window = glfwCreateWindow(configState.screenWidth, configState.screenHeight, "Project 2", nullptr, nullptr);
    if (window == nullptr)
	{
        std::cerr << "Failed to create GLFW window\n";
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);
	//glViewport(0, 0, 800, 600);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetKeyCallback(window, key_callback);
	//GLEW: check errors
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		std::string str = reinterpret_cast<const char*>(glewGetErrorString(err));
#ifdef USE_STD_FORMAT
		std::cerr << std::format("Error: {}\n", str);
#else
        std::cerr << "Error: " << str << "\n";
#endif
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	std::string str = reinterpret_cast<const char*>(glewGetString(GLEW_VERSION));
#ifdef USE_STD_FORMAT
	std::cout <<  std::format("Status: Using GLEW {}\n", str);
#else
    std::cout << "Status: Using GLEW " << str << "\n";
#endif
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_STENCIL_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_PROGRAM_POINT_SIZE_EXT);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	return window;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	auto* ptr = static_cast<bf::GlfwStruct*>(glfwGetWindowUserPointer(window));
	glViewport(0, 0, width, height);
    if(!ptr)
        return;
    ptr->configState.screenWidth=width;
    ptr->configState.screenHeight=height;
}

void blockAxes(glm::vec3& v, uint8_t axisLock) {
    if(axisLock&0x01)
        v.x=0.f;
    if(axisLock&0x02)
        v.y=0.f;
    if(axisLock&0x04)
        v.z=0.f;
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    //TODO - organize this file
    //set data
	auto* ptr = static_cast<bf::GlfwStruct*>(glfwGetWindowUserPointer(window));
    if(!ptr)
        return;
    bf::GlfwStruct& s = *ptr;
    bf::Camera& camera = s.scene.camera;
    const float& deltaTime = s.configState.deltaTime;
    bf::ConfigState& configState = s.configState;
    bf::Transform& multiTransform = s.scene.multiCursor.transform;
    bf::Cursor& cursor = s.scene.cursor;
	auto& scene = s.scene;
	auto& objectArray = ptr->scene.objectArray;

    configState.mouseX = static_cast<float>(xposIn);
	configState.mouseY = static_cast<float>(yposIn);
    const auto& xpos = configState.mouseX;
    const auto& ypos = configState.mouseY;
	static bool firstMouse=true;
	static float lastX=static_cast<float>(s.configState.screenWidth) * .5f;
	static float lastY=static_cast<float>(s.configState.screenHeight) * 0.5f;

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
    //events
	objectArray.onMouseMove({lastX,lastY},{xpos,ypos});

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	float speed = configState.movementSpeed * deltaTime;
	float rotSpeed = configState.rotationSpeed * deltaTime;
	float scaleSpeed = .5f * deltaTime;

	if(configState.state != bf::None) {
		float myVec[] = {.0f,.0f,.0f};
		if(configState.isAltPressed)
			myVec[2] = xoffset;
		else
		{
			myVec[0] = xoffset;
			myVec[1] = yoffset;
		}
		auto glmVec = glm::vec3(myVec[0],myVec[1],myVec[2]);
		if (configState.isCtrlPressed) {
			//camera movement
			if(configState.state == bf::MiddleClick) {
				camera.position += bf::rotate(speed * glmVec, camera.rotation);
			}
			else if(configState.state == bf::RightClick) {
				std::swap(glmVec[0],glmVec[1]);
                glm::vec3 centre;
                if(objectArray.isMultipleActive())
                    centre = multiTransform.position;
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
                glm::vec3 vec;
                if(configState.isUniformScaling) {
                    float vecValue = scaleSpeed * ((glmVec[0]+glmVec[1])*.5f+glmVec[2]);
                    vec = {vecValue ,vecValue, vecValue};
                }
                else {
                    vec = bf::rotate(scaleSpeed * glmVec, camera.rotation);
                    blockAxes(vec, configState.isAxesLocked);
                }
                std::array<bool,3> sgn = {std::signbit(deltaTransform.scale.x),std::signbit(deltaTransform.scale.y),std::signbit(deltaTransform.scale.z)};
				deltaTransform.scale += vec;
                if(std::abs(deltaTransform.scale.x)<0.001f)
                    deltaTransform.scale.x = sgn[0] ? -0.001f : 0.001f;
                if(std::abs(deltaTransform.scale.y)<0.001f)
                    deltaTransform.scale.y = sgn[0] ? -0.001f : 0.001f;
                if(std::abs(deltaTransform.scale.z)<0.001f)
                    deltaTransform.scale.z = sgn[0] ? -0.001f : 0.001f;
			} else if (configState.state == bf::MiddleClick) {
                auto vec = bf::rotate(speed * glmVec, camera.rotation);
                blockAxes(vec, configState.isAxesLocked);
				deltaTransform.position += vec;
			} else if (configState.state == bf::RightClick) {
				std::swap(glmVec[0],glmVec[1]);
                auto vec = bf::rotate(rotSpeed * glmVec, camera.rotation);
                blockAxes(vec, configState.isAxesLocked);
				deltaTransform.rotation += vec;
			}
			//object movement
            bf::Transform t;
            if(objectArray.isMultipleActive())
                t = multiTransform;
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
						objectArray[i].setNewTransform(scene.objectArray.getCentre(), multiTransform, t);
					}
				}
                multiTransform = std::move(t);
			}
			else if(objectArray.isMovable(objectArray.getActiveIndex())) {
                objectArray[objectArray.getActiveIndex()].setTransform(std::move(t));
			}
			else {
                glm::vec3 screenPos = bf::toScreenPos(configState.screenWidth,configState.screenHeight,cursor.transform.position,scene.getView(),scene.getProjection());
                screenPos.x = xpos;
                screenPos.y = ypos;
                cursor.transform.position = bf::toGlobalPos(configState.screenWidth,configState.screenHeight,screenPos, scene.getInverseView(), scene.getInverseProjection());
			}
		}
	}
}

void scroll_callback(GLFWwindow* window, double /*xoffset*/, double yoffset)
{
    auto* ptr = static_cast<bf::GlfwStruct*>(glfwGetWindowUserPointer(window));
    if(!ptr)
        return;
    auto& s = *ptr;
    auto yOffsetF = static_cast<float>(yoffset);
    s.configState.cameraFOV = std::max(std::min(s.configState.cameraFOV - yOffsetF, s.configState.getCameraFoVmax()),
									   s.configState.getCameraFoVmin());
}

void key_callback(GLFWwindow* window, int k, int /*scancode*/, int action, int mods) {
    //get data from pointer
	auto* ptr = static_cast<bf::GlfwStruct*>(glfwGetWindowUserPointer(window));
    if(!ptr || ptr->io.io.WantCaptureKeyboard)
        return;
    auto& s = *ptr;
    //cast data
    using namespace bf::event;
    auto key = static_cast<Key>(k);
    auto modKeyBit = static_cast<ModifierKeyBit>(mods);
    auto state = static_cast<KeyState>(action);
    //delegate event
    if(state==bf::event::KeyState::Press) {
		if (key == Key::Escape) {
			glfwSetWindowShouldClose(window, true);
		}
		if(s.scene.onKeyPressed(key, modKeyBit, s.configState))
			return;
        s.configState.onKeyPressed(key, modKeyBit);
	}
	else {
		if(s.scene.onKeyReleased(key, modKeyBit, s.configState))
			return;
        s.configState.onKeyReleased(key, modKeyBit);
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    //get data from pointer
    auto* ptr = static_cast<bf::GlfwStruct*>(glfwGetWindowUserPointer(window));
    if(!ptr || ptr->io.io.WantCaptureMouse)
        return;
    auto& s = *ptr;
    //cast data
    using namespace bf::event;
    auto mouseButton = static_cast<MouseButton>(button);
    auto modKeyBit = static_cast<ModifierKeyBit>(mods);
    auto state = static_cast<MouseButtonState>(action);
    //delegate event
	if(state==bf::event::MouseButtonState::Press) {
		if(s.scene.onMouseButtonPressed(mouseButton, modKeyBit, s.configState))
			return;
        s.configState.onMouseButtonPressed(mouseButton, modKeyBit);
	}
	else {
		if(s.scene.onMouseButtonReleased(mouseButton, modKeyBit, s.configState))
			return;
        s.configState.onMouseButtonReleased(mouseButton, modKeyBit);
	}
}