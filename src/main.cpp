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

#include "Settings.h"
#include "ImGuiUtil.h"
#include "GlfwUtil.h"
#include "Shader.h"
#include "camera.h"
#include "Solids/Torus.h"
#include "Solids/Point.h"
#include "Util.h"
#include <memory>
#include "Curves/BezierCurve.h"
#include "Scene.h"
#include "Curves/BezierCurve2.h"

const std::string SHADER_PATH = "../shaders/";

int main() {
    bf::Settings settings;
	GLFWwindow* window=bf::glfw::init(settings);
	if(!window) {
        fprintf(stderr, "Fatal error creating GLFW window\n");
		return EXIT_FAILURE;
	}
	//init ImGUI
    //structures
	ImGuiIO& io = bf::imgui::init(window);
    //bf::Camera camera(0.1f,100.f,glm::vec3(0.0f, 0.0f, -10.0f),glm::vec3(0.0f,0.0f,0.f));
	bf::Scene scene(settings.aspect,glm::vec3(0.0f, 0.0f, -10.0f),glm::vec3(0.0f,0.0f,0.f),0.1f,100.f);
	bf::Transform& multiTransform=scene.multiCursor.transform;
    float deltaTime = 0.0f;
	bf::Shader shader(SHADER_PATH+"shader.vert", SHADER_PATH+"shader.frag");

    bf::GlfwStruct glfwStruct(settings,scene,deltaTime,io);
    glfwSetWindowUserPointer(window,&glfwStruct);
	//bool show_another_window = false;

	scene.objectArray.add<bf::Torus>();
	bf::Point::initObjArrayRef(scene.objectArray);
	bf::BezierCommon::initData(scene, settings, window);
	//objects.emplace_back(new bf::Torus());

	while (!glfwWindowShouldClose(window))
	{
		static float lastFrame;
		auto currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		// Poll and handle events (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		glfwPollEvents();
		bf::glfw::processInput(window);

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
		///CREATE OBJECT PANEL
		ImGui::Begin("Create object");
		//ImGui::
		if(ImGui::Button("Torus")) {
			scene.objectArray.add<bf::Torus>(scene.cursor.transform);
		}
		if(ImGui::Button("Point")) {
			scene.objectArray.add<bf::Point>(scene.cursor.transform);
			int redIndex = scene.objectArray.getActiveRedirector();
			if(scene.objectArray.isCorrect(redIndex)) {
				scene.objectArray[redIndex].addPoint(scene.objectArray.size()-1);
			}
		}
		if(ImGui::Button("Bézier curve 0")) {
			scene.objectArray.addRef<bf::BezierCurve>();
		}
        if(ImGui::Button("Bézier curve 2")) {
            scene.objectArray.addRef<bf::BezierCurve2>();
        }
		ImGui::End();
		//ImGui
		///LIST OF OBJECTS PANEL
		ImGui::Begin("List of objects");
		ImGui::Text("Hold CTRL and click to select multiple items.");

		if(ImGui::Checkbox("Multipled clicked objects", &settings.isMultiState)) {
			if(!settings.isMultiState)
				scene.objectArray.clearSelection();
		}
		ImGui::Checkbox("Uniform scalng", &settings.isUniformScaling);
		for(int i=0;i<3;i++) {
			static bool tmp[3];
			constexpr char axisName[] = {'X', 'Y', 'Z'};
			tmp[i]=settings.isAxesLocked&(0x1<<i);
			if(ImGui::Checkbox((std::string("Lock ")+axisName[i]).c_str(), &tmp[i])) {
				if (tmp[i])
					settings.isAxesLocked += 0x1 << i;
				else
					settings.isAxesLocked -= 0x1 << i;
			}
		}
		if(ImGui::Button("Delete")) {
			scene.objectArray.removeActive();
		}
		if(ImGui::Button("Clear selection"))
			scene.objectArray.clearSelection(-1);
		for (std::size_t n = 0u; n < scene.objectArray.size(); n++)
		{
			if(!scene.objectArray.isCorrect(n)) {
				ImGui::Text("Empty unique pointer");
				continue;
			}
			if (scene.objectArray.imGuiCheckChanged(n))
			{
                multiTransform = bf::Transform::Default;
				if (!settings.isMultiState) { // Clear selection when CTRL is not held
					scene.objectArray.clearSelection(n);
				}
			}
		}
		ImGui::End();
		/// MODIFY OBJECT PANEL
		ImGui::Begin("Modify object");
		//active object settings
		if(scene.objectArray.getActiveIndex()!=-1 && !settings.isMultiState)
			scene.objectArray[scene.objectArray.getActiveIndex()].ObjectGui();
		else {
			bool isAny = false;
			if (settings.isMultiState) {
				for (std::size_t i = 0u; i < scene.objectArray.size(); i++) {
					if (scene.objectArray.isCorrect(i) && scene.objectArray.isActive(i)) {
						isAny=true;
						break;
					}
				}
				if(isAny) {
					ImGui::Text("Multiple objects");
					bf::Transform oldTransform = multiTransform;
					bool tmp = bf::imgui::checkChanged("Object position", multiTransform.position);
					tmp = bf::imgui::checkChanged("Object rotation", multiTransform.rotation) || tmp;
					tmp = bf::imgui::checkChanged("Object scale", multiTransform.scale, true) || tmp;
					if (tmp) {
						for (std::size_t i = 0; i < scene.objectArray.size(); i++) {
							if (scene.objectArray.isCorrect(i) && scene.objectArray.isActive(i)) {
								scene.objectArray[i].setNewTransform(scene.objectArray.getCentre(), oldTransform, multiTransform);
							}
						}
					}
				}
			}
			if(!isAny) {
				scene.cursor.ObjectGui(window, scene.getView(), scene.getInverseView(), scene.getProjection(), scene.getInverseProjection());
			}
		}
		ImGui::End();
		//camera info and set cursor
		///CAMERA INFO PANEL
		ImGui::Begin("Camera info");
		scene.camera.ObjectGui();
		ImGui::End();
		// Rendering
		ImGui::Render();
		//OpenGL draw
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		scene.draw(shader, settings, display_w, display_h);
		//end of OpenGL draw
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);

	}
	bf::imgui::destroy();
	bf::glfw::destroy(window);
	return 0;
}