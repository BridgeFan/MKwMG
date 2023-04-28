//
// Created by kamil-hp on 13.03.2022.
//

#ifndef MG1_ZAD2_WINDOW_H
#define MG1_ZAD2_WINDOW_H
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include "Scene.h"
#include "ImGui/ImGuiUtil.h"

class ImGuiIO;
struct GLFWwindow;
namespace bf {
    struct ConfigState;
    class Scene;
    struct Transform;
    struct GlfwStruct {
        bf::ConfigState &configState;
		bf::imgui::IO io;
        bf::Scene scene;
        GlfwStruct(bf::ConfigState &configState1, GLFWwindow* window);
    };
	class Window {
    private:
        bf::GlfwStruct* glfwStruct;
		GLFWwindow* window;
    public:
        explicit Window(const bf::ConfigState& configState);
        void run(bf::ConfigState &configState);
        ~Window();
	};
}
//callbacks

#endif //MG1_ZAD2_WINDOW_H
