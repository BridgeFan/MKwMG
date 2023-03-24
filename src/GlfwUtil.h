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
	class Settings;
	class Camera;
	class Transform;
	namespace glfw {
		GLFWwindow* init(bf::Settings& settings);
		void destroy(GLFWwindow* window);
		void processInput(GLFWwindow *window);
		glm::vec3 toScreenPos(GLFWwindow *window, const glm::vec3 &worldPos, const glm::mat4 &view, const glm::mat4 &projection);
		glm::vec3 toGlobalPos(GLFWwindow *window, const glm::vec3 &mousePos, const glm::mat4 &inverseView, const glm::mat4 &inverseProjection);
	}
}
//callbacks

#endif //MG1_ZAD2_GLFWUTIL_H
