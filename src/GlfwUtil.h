//
// Created by kamil-hp on 13.03.2022.
//

#ifndef MG1_ZAD2_GLFWUTIL_H
#define MG1_ZAD2_GLFWUTIL_H
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
class ImGuiIO;
struct GLFWwindow;
namespace bf {
	class ConfigState;
	class Camera;
	class Transform;
	namespace glfw {
		GLFWwindow* init(const bf::ConfigState& configState);
		void destroy(GLFWwindow* window);
	}
}
//callbacks

#endif //MG1_ZAD2_GLFWUTIL_H
