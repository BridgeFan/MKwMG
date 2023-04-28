#include <GL/glew.h>
#include "imgui-master/imgui.h"
#include "imgui-master/backends/imgui_impl_glfw.h"
#include "imgui-master/backends/imgui_impl_opengl3.h"
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers
#include <cstdlib>

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

#include "ConfigState.h"
#include "ImGui/ImGuiUtil.h"
#include "ImGui/ImGuiPanel.h"
#include "GlfwUtil.h"
#include "Solids/Torus.h"
#include "Solids/Point.h"
#include "Util.h"
#include <iostream>
#include "Scene.h"
#include "ShaderArray.h"
#include "FileLoading.h"

const std::string SHADER_PATH = "../shaders/";

int main() {
    bf::ConfigState configState;
    ///INIT GL AND GLFW
	GLFWwindow* window=bf::glfw::init(configState);
	if(!window) {
        std::cerr <<  "Fatal error creating GLFW window\n";
		return EXIT_FAILURE;
	}
    bf::ShaderArray shaderArray;
    shaderArray.addBasicShader(SHADER_PATH+"shader", false);
    shaderArray.addTessellationShader(SHADER_PATH+"bezierShader", false);
    ///INIT MY STRUCTURES
	ImGuiIO& io = bf::imgui::init(window);
	bf::Scene scene(static_cast<float>(configState.screenWidth)/static_cast<float>(configState.screenHeight),
                    glm::vec3(0.0f, 0.0f, -10.0f),glm::vec3(0.0f,0.0f,0.f),
                    configState.cameraZoom,0.1f,100.f);
    float deltaTime = 0.0f;
    bf::GlfwStruct glfwStruct(configState,scene,deltaTime,io);
    glfwSetWindowUserPointer(window,&glfwStruct);
    bf::Point::initObjArrayRef(scene.objectArray);
    bf::Object::initData(configState, scene);
    //initial objectArray
	scene.objectArray.add<bf::Torus>();

	while (!glfwWindowShouldClose(window))
	{
		deltaTime = bf::getDeltaTime();
        //begin
        glfwPollEvents();
        ///IMGUI START
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		///IMGUI
		bf::imgui::createObjectPanel(scene);
        bf::imgui::listOfObjectsPanel(scene,configState);
        bf::imgui::modifyObjectPanel(scene,configState);
		bf::imgui::cameraInfoPanel(scene, configState);
		///RENDER
		ImGui::Render();
		scene.draw(shaderArray, configState);
		///FINISH
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);

	}
	bf::imgui::destroy();
	bf::glfw::destroy(window);
	return 0;
}