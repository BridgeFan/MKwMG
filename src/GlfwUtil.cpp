//
// Created by kamil-hp on 13.03.2022.
//
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "GlfwUtil.h"
#include "Settings.h"
#include "camera.h"
#include <array>
#include "Solids/Object.h"
#include "../../imgui-master/imgui.h"
#include "Util.h"
#include "Solids/Cursor.h"

constexpr int SRC_WIDTH=1024;
constexpr int SRC_HEIGHT=768;

//declaration of functions
GLFWwindow* initWindow(bf::Settings& settings);

void framebuffer_size_callback(GLFWwindow*, int width, int height);
void mouse_callback(GLFWwindow*, double xposIn, double yposIn);
void scroll_callback(GLFWwindow*, double /*xoffset*/, double yoffset);
void key_callback(GLFWwindow*, int key, int /*scancode*/, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);


GLFWwindow* bf::glfw::init(bf::Settings& settings) {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if defined(__APPLE__) || defined (__MACH__)
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	GLFWwindow* window=initWindow(settings);
	glfwSwapInterval(1);
	return window;
}

void bf::glfw::destroy(GLFWwindow* window) {
	glfwDestroyWindow(window);
	glfwTerminate();
}
//internal functions

GLFWwindow* initWindow(bf::Settings& settings)
{
	settings.aspect=(float)SRC_WIDTH/(float)SRC_HEIGHT;
	GLFWwindow* window = glfwCreateWindow(SRC_WIDTH, SRC_HEIGHT, "Project 2", nullptr, nullptr);
	if (window == nullptr)
	{
        fprintf(stderr, "Failed to create GLFW window\n");
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
	//glfwSetCursorPosCallback(window, mouse_callback);
	//GLEW: check errors
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_PROGRAM_POINT_SIZE_EXT);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//system("pwd");
	return window;
}

glm::vec3 bf::glfw::toScreenPos(GLFWwindow* window, const glm::vec3& worldPos, const glm::mat4& view, const glm::mat4& projection) {
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    auto v = projection*view*glm::vec4(worldPos,1.f);
    v/=v.w;
    v.x=(v.x+1.f)*(float)width*.5f;
    v.y=(1.f-v.y)*(float)height*.5f;
    return {v.x,v.y,v.z};
}

glm::vec3 bf::glfw::toGlobalPos(GLFWwindow* window, const glm::vec3& mousePos, const glm::mat4& inverseView, const glm::mat4& inverseProjection) {
    auto mp = mousePos;
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    mp.x = 2.f*mp.x/(float)width-1.f;
    mp.y = 1.f-2.f*mp.y/(float)height;
    auto v = inverseView*inverseProjection*glm::vec4(mp,1.f);
    v/=v.w;
    return {v.x,v.y,v.z};
}

void bf::glfw::processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	/*if(io.WantCaptureKeyboard)
		return;*/
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    auto ptr = (bf::GlfwStruct*)glfwGetWindowUserPointer(window);
	glViewport(0, 0, width, height);
	ptr->settings.aspect=(float)width/(float)height;
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
    auto* ptr = (bf::GlfwStruct*)glfwGetWindowUserPointer(window);
    bf::Camera& camera = ptr->camera;
    const float& deltaTime = ptr->deltaTime;
    const bf::Settings& settings = ptr->settings;
    bf::Transform& multiTransform = ptr->multiTransform;
    glm::vec3& multiCentre = ptr->multiCentre;
    bf::Cursor& cursor = ptr->cursor;
    auto& selection = ptr->selection;
    auto& objects = ptr->objects;

	auto xpos = static_cast<float>(xposIn);
	auto ypos = static_cast<float>(yposIn);
	static bool firstMouse=true;
	static float lastX=SRC_WIDTH / 2.0f;
	static float lastY=SRC_HEIGHT / 2.0f;

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	float speed = camera.MovementSpeed * deltaTime;
	float rotSpeed = camera.RotationSpeed * deltaTime;
	float scaleSpeed = .5f * deltaTime;

	/*if(settings.state == MiddleClick) {
		//move right or up
		camera.Position += speed * (camera.getRight() * xoffset + camera.getUp() * yoffset);
	}
	else if(settings.state == RightClick) {
		//rotate
		camera.ProcessMouseMovement(xoffset, yoffset);
	}*/
	//uint8_t axis1, axis2;
	//uint8_t axis3 = UINT8_MAX;
	/*switch(settings.isAxesLocked) {
		case 0:
			axis1 = 0;
			axis2 = 1;
			axis3 = 2;
			isRotationInverted=true;
			break;
		case 1: //X locked
			axis1 = 1;
			axis2 = 2;
			break;
		case 2: //Y locked
			axis1 = 2;
			axis2 = 0;
			break;
		case 3: //X and Y locked
			axis1 = 2;
			axis2 = 2;
			break;
		case 4: //Z locked
			axis1 = 0;
			axis2 = 1;
			isRotationInverted=true;
			break;
		case 5: //X and Z locked
			axis1 = 1;
			axis2 = 1;
			break;
		case 6: //Y and Z locked
			axis1 = 0;
			axis2 = 0;
			break;
		case 7: //X, Y and Z locked
			axis1 = UINT8_MAX; //ignore moving and rotating
			break;
	}*/
	if(settings.state != bf::None) {
		float myVec[] = {.0f,.0f,.0f};
		if(settings.isAltPressed)
			myVec[2] = xoffset;
		else
		{
			myVec[0] = xoffset;
			myVec[1] = yoffset;
		}
		auto glmVec = glm::vec3(myVec[0],myVec[1],myVec[2]);
		if (settings.isCtrlPressed) {
			//camera movement
			if(settings.state == bf::MiddleClick) {
				camera.position += bf::rotate(speed * glmVec, camera.rotation);
			}
			else if(settings.state == bf::RightClick) {
				std::swap(glmVec[0],glmVec[1]);
                glm::vec3 centre;
                if(settings.isMultiState)
                    centre = multiTransform.position;
                else if(settings.activeIndex>=0 && settings.activeIndex<(int)objects.size() && objects[settings.activeIndex])
                    centre = objects[settings.activeIndex]->getPosition();
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
			if (settings.state != bf::None && settings.isShiftPressed) {
                glm::vec3 vec;
                if(settings.isUniformScaling) {
                    float vecValue = scaleSpeed * ((glmVec[0]+glmVec[1])*.5f+glmVec[2]);
                    vec = {vecValue ,vecValue, vecValue};
                }
                else {
                    vec = bf::rotate(scaleSpeed * glmVec, camera.rotation);
                    blockAxes(vec, settings.isAxesLocked);
                }
                std::array<bool,3> sgn = {std::signbit(deltaTransform.scale.x),std::signbit(deltaTransform.scale.y),std::signbit(deltaTransform.scale.z)};
				deltaTransform.scale += vec;
                if(std::abs(deltaTransform.scale.x)<0.001f)
                    deltaTransform.scale.x = sgn[0] ? -0.001f : 0.001f;
                if(std::abs(deltaTransform.scale.y)<0.001f)
                    deltaTransform.scale.y = sgn[0] ? -0.001f : 0.001f;
                if(std::abs(deltaTransform.scale.z)<0.001f)
                    deltaTransform.scale.z = sgn[0] ? -0.001f : 0.001f;
			} else if (settings.state == bf::MiddleClick) {
                auto vec = bf::rotate(speed * glmVec, camera.rotation);
                blockAxes(vec, settings.isAxesLocked);
				deltaTransform.position += vec;
			} else if (settings.state == bf::RightClick) {
				std::swap(glmVec[0],glmVec[1]);
                auto vec = bf::rotate(rotSpeed * glmVec, camera.rotation);
                blockAxes(vec, settings.isAxesLocked);
				deltaTransform.rotation += vec;
			}
			//object movement
            bf::Transform t;
            if(settings.isMultiState)
                t = multiTransform;
            else if(settings.activeIndex>=0 && settings.activeIndex<(int)objects.size() && objects[settings.activeIndex])
                t = objects[settings.activeIndex]->getTransform();
            else
                t = cursor.transform;
            t.position += deltaTransform.position;
            t.rotation += deltaTransform.rotation;
            t.scale += deltaTransform.scale;
			if(settings.isMultiState) {
				for (int i = 0; i < (int)objects.size(); i++) {
					if (objects[i] && selection[i]) {
						objects[i]->setNewTransform(multiCentre, multiTransform, t);
					}
				}
                multiTransform = std::move(t);
				//multiCentre = getCentre(selection, objects);
			}
			else if(settings.activeIndex>=0 && settings.activeIndex<(int)objects.size() && objects[settings.activeIndex]) {
                objects[settings.activeIndex]->setTransform(std::move(t));
			}
			else {
                glm::vec3 screenPos = bf::glfw::toScreenPos(window,cursor.transform.position,settings.View,settings.Projection);
                screenPos.x = xpos;
                screenPos.y = ypos;
                cursor.transform.position = bf::glfw::toGlobalPos(window,screenPos, settings.InverseView, settings.InverseProjection);
			}
		}
	}
}

void scroll_callback(GLFWwindow* window, double /*xoffset*/, double yoffset)
{
    auto* ptr = (bf::GlfwStruct*)glfwGetWindowUserPointer(window);
	ptr->camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void key_callback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/) {
    auto* ptr = (bf::GlfwStruct*)glfwGetWindowUserPointer(window);
    bf::Settings& settings = ptr->settings;
    auto& selection = ptr->selection;
    auto& objects = ptr->objects;
	if(ptr->io.WantCaptureKeyboard)
		return;
	if(key==GLFW_KEY_LEFT_CONTROL || key==GLFW_KEY_RIGHT_CONTROL) {
		settings.isCtrlPressed = (action == GLFW_PRESS);
	}
	if(key==GLFW_KEY_LEFT_ALT || key==GLFW_KEY_RIGHT_ALT || key==GLFW_KEY_A) { //A is additional ALT
		settings.isAltPressed = (action == GLFW_PRESS);
	}
	if(key==GLFW_KEY_LEFT_SHIFT || key==GLFW_KEY_RIGHT_SHIFT) {
		settings.isShiftPressed = (action == GLFW_PRESS);
	}
	if(action == GLFW_PRESS) {
		switch(key) {
			case GLFW_KEY_M:
				settings.isMultiState = !settings.isMultiState;
				break;
			case GLFW_KEY_ESCAPE:
				glfwWindowShouldClose(window);
				break;
			case GLFW_KEY_X:
				if(settings.isAxesLocked%2)
					settings.isAxesLocked -= 0x1;
				else
					settings.isAxesLocked += 0x1;
				break;
			case GLFW_KEY_Y:
				if((settings.isAxesLocked>>1)%2)
					settings.isAxesLocked -= 0x2;
				else
					settings.isAxesLocked += 0x2;
				break;
			case GLFW_KEY_Z:
				if((settings.isAxesLocked>>2)%2)
					settings.isAxesLocked -= 0x4;
				else
					settings.isAxesLocked += 0x4;
				break;
			case GLFW_KEY_C:
				clearSelection(selection, -1, settings);
                ptr->multiCentre = bf::getCentre(selection, objects);
				break;
            case GLFW_KEY_U:
                settings.isUniformScaling = !settings.isUniformScaling;
                break;
			case GLFW_KEY_DELETE:
				deleteObjects(selection, objects);
                ptr->multiCentre = bf::getCentre(selection, objects);
				break;
			default:
				;
		}
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    auto* ptr = (bf::GlfwStruct*)glfwGetWindowUserPointer(window);
    bf::Settings& settings = ptr->settings;
    auto& selection = ptr->selection;
    auto& objects = ptr->objects;
	if(ptr->io.WantCaptureMouse)
		return;
	if(action == GLFW_RELEASE && ((button == GLFW_MOUSE_BUTTON_RIGHT && settings.state == bf::RightClick) ||
		(button == GLFW_MOUSE_BUTTON_MIDDLE && settings.state == bf::MiddleClick)))
		settings.state = bf::None;
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
		settings.state = bf::RightClick;
	if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
		settings.state = bf::MiddleClick;
	if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		double mouseX, mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);
		constexpr float sqrDist = 64.f;
        int selectionIndex = -1;
        float actualZ = 9.999f;
		for(unsigned i=0u;i<objects.size();i++) {
			if(!objects[i]/* || typeid(*objects[i])!=typeid(Point)*/)
				continue;
            auto screenPos = bf::glfw::toScreenPos(window, objects[i]->getTransform().position, settings.View, settings.Projection);
            float d = (screenPos.x-(float)mouseX)*(screenPos.x-(float)mouseX)+(screenPos.y-(float)mouseY)*(screenPos.y-(float)mouseY);
            //float d = glm::length2(glm::cross(objects[i]->getPosition()-camera.position, ray))/glm::length2(ray);
			if(d<=sqrDist && actualZ>screenPos.z) {
                selectionIndex=(int)i;
                actualZ=screenPos.z;
				/*if(!(mods&GLFW_MOD_CONTROL))
					clearSelection(selection, i, settings);
				selection[i] = true;
                ptr->multiCentre = getCentre(selection, objects);
				break;*/
			}
		}
        if(selectionIndex >= 0) {
            if(!(mods&GLFW_MOD_CONTROL))
                bf::clearSelection(selection, selectionIndex, settings);
            selection[selectionIndex] = true;
            settings.activeIndex = selectionIndex;
            ptr->multiCentre = bf::getCentre(selection, objects);
        }
	}
}