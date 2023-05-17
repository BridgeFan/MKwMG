//
// Created by kamil-hp on 13.03.2022.
//
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Window.h"
#include "ConfigState.h"
#include <array>
#include "ImGui/imgui_include.h"
#include "Util.h"
#include <iostream>
#include <format>
#include "Event.h"
#include "ImGui/ImGuiUtil.h"
#include "ImGui/ImGuiPanel.h"


//declaration of functions
GLFWwindow* initWindow(const bf::ConfigState& configState);
void framebuffer_size_callback(GLFWwindow*, int width, int height);
void mouse_callback(GLFWwindow*, double xposIn, double yposIn);
void scroll_callback(GLFWwindow*, double /*xoffset*/, double yoffset);
void key_callback(GLFWwindow*, int key, int /*scancode*/, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void GLAPIENTRY MessageCallback( GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei, const GLchar* message, const void* );

bf::GlfwStruct::GlfwStruct(bf::ConfigState &configState1, GLFWwindow* window) : configState(configState1), io(window, configState1), scene(configState1) {}

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
    std::cout << "Running window\n";
	while (!glfwWindowShouldClose(window))
	{
		configState.deltaTime = bf::getDeltaTime();
		///IMGUI
		bf::imgui::preDraw();
		bf::imgui::createObjectPanel(scene,configState);
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
    std::cout << "Init window begin\n";
    GLFWwindow* window;
    if(configState.screenFullscreen) {
        window = glfwCreateWindow(configState.screenWidth, configState.screenHeight, "Project 2", glfwGetPrimaryMonitor(), nullptr);
    }
    else {
        window = glfwCreateWindow(configState.screenWidth, configState.screenHeight, "Project 2", nullptr, nullptr);
    }
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
		std::cerr << std::format("Error: {}\n", str);
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	std::string str = reinterpret_cast<const char*>(glewGetString(GLEW_VERSION));
	std::cout <<  std::format("Status: Using GLEW {}\n", str);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_PROGRAM_POINT_SIZE_EXT);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::cout << "Init window end\n";
	return window;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	auto* ptr = static_cast<bf::GlfwStruct*>(glfwGetWindowUserPointer(window));
    if(!ptr)
        return;
    if(ptr->configState.stereoscopic)
        return;
    glViewport(0, 0, width, height);
    auto& s = ptr->scene;
    auto& c = ptr->configState;
    if(c.screenWidth==width && c.screenHeight==height)
        return;
    c.screenWidth=width;
    c.screenHeight=height;
    s.resizeFramebuffers(width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	auto* ptr = static_cast<bf::GlfwStruct*>(glfwGetWindowUserPointer(window));
    if(!ptr)
        return;
    bf::GlfwStruct& s = *ptr;
    //set differences
    s.configState.mouseX = static_cast<float>(xposIn);
    s.configState.mouseY = static_cast<float>(yposIn);
    s.io.io.AddMousePosEvent(s.configState.mouseX, s.configState.mouseY);
    if(s.io.io.WantCaptureMouse) {
        return;
    }
	static glm::vec2 lastMousePos = {s.configState.mouseX, s.configState.mouseY};
    //events
	s.scene.onMouseMove(lastMousePos, s.configState);
	lastMousePos = {s.configState.mouseX, s.configState.mouseY};
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    auto* ptr = static_cast<bf::GlfwStruct*>(glfwGetWindowUserPointer(window));
    if(!ptr)
        return;
    auto& s = *ptr;
    s.io.io.AddMouseWheelEvent(xoffset, yoffset);
    if(s.io.io.WantCaptureMouse) {
        return;
    }
    auto yOffsetF = static_cast<float>(yoffset);
    s.configState.cameraFOV = std::max(std::min(s.configState.cameraFOV - yOffsetF, s.configState.getCameraFoVmax()),
									   s.configState.getCameraFoVmin());
}

void key_callback(GLFWwindow* window, int k, int /*scancode*/, int action, int mods) {
    //get data from pointer
	auto* ptr = static_cast<bf::GlfwStruct*>(glfwGetWindowUserPointer(window));
    if(!ptr) return;
    auto& s = *ptr;
    //cast data
    using namespace bf::event;
    auto key = static_cast<Key>(k);
    auto modKeyBit = static_cast<ModifierKeyBit>(mods);
    auto state = static_cast<KeyState>(action);
    using namespace bf::event;
    s.io.io.AddKeyEvent(toImGui(key),toImGui(state));
    if(s.io.io.WantCaptureKeyboard)
        return;
    //delegate event
    if(state==KeyState::Press) {
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
    if(!ptr)
        return;
    auto& s = *ptr;
    //cast data
    using namespace bf::event;
    auto mouseButton = static_cast<MouseButton>(button);
    auto modKeyBit = static_cast<ModifierKeyBit>(mods);
    auto state = static_cast<MouseButtonState>(action);
    s.io.io.AddMouseButtonEvent(bf::event::toImGui(mouseButton),bf::event::toImGui(state));
    if(s.io.io.WantCaptureMouse) {
        return;
    }
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
	std::cerr << std::format("GL CALLBACK: id={}, source = {}, type = {}, severity = {}, message = {}\n",
							 id, sourceStr, typeStr, severityStr, message );
}