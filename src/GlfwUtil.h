//
// Created by kamil-hp on 13.03.2022.
//

#ifndef MG1_ZAD2_GLFWUTIL_H
#define MG1_ZAD2_GLFWUTIL_H
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

struct GLFWwindow;
class Settings;
class Camera;
class ImGuiIO;
class Transform;

GLFWwindow* initGlfw(Settings& settings);
void destroyGlfw(GLFWwindow* window);
void processInput(GLFWwindow *window, Camera& camera, const ImGuiIO& io);
glm::vec3 toScreenPos(GLFWwindow* window, const glm::vec3& worldPos, const glm::mat4& view, const glm::mat4& projection);
glm::vec3 toGlobalPos(GLFWwindow* window, const glm::vec3& mousePos, const glm::mat4& inverseView, const glm::mat4& inverseProjection);
//callbacks

#endif //MG1_ZAD2_GLFWUTIL_H
