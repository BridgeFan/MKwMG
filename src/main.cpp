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
#include "camera.h"
#include "Solids/Torus.h"
#include "Solids/Point.h"
#include "Util.h"
#include <iostream>
#include "Curves/BezierCurve.h"
#include "Scene.h"
#include "Curves/BezierCurve2.h"
#include "Curves/BezierCurveInter.h"
#include "ShaderArray.h"
#include "FileLoading.h"
#include <format>

const std::string SHADER_PATH = "../shaders/";

void GLAPIENTRY
MessageCallback( GLenum source,
				 GLenum type,
				 GLuint id,
				 GLenum severity,
				 GLsizei,
				 const GLchar* message,
				 const void* )
{
	std::string sourceStr;
	switch (source)
	{
		case GL_DEBUG_SOURCE_API:				sourceStr="API"; break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:		sourceStr="Window System"; break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER:	sourceStr="Shader Compiler"; break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:		sourceStr="Third Party"; break;
		case GL_DEBUG_SOURCE_APPLICATION:		sourceStr="Application"; break;
		case GL_DEBUG_SOURCE_OTHER: default:	sourceStr="Other"; break;
	}
	std::string typeStr;
	switch (type)
	{
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

int main() {
    bf::Settings settings;
	GLFWwindow* window=bf::glfw::init(settings);
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback,nullptr);
	if(!window) {
        std::cerr <<  "Fatal error creating GLFW window\n";
		return EXIT_FAILURE;
	}
	//init ImGUI
    //structures
	ImGuiIO& io = bf::imgui::init(window);
	bf::Scene scene(settings.aspect,glm::vec3(0.0f, 0.0f, -10.0f),glm::vec3(0.0f,0.0f,0.f),0.1f,100.f);
	bf::Transform& multiTransform=scene.multiCursor.transform;
    float deltaTime = 0.0f;
    bf::ShaderArray shaderArray;
    shaderArray.addBasicShader(SHADER_PATH+"shader", false);
    shaderArray.addTessellationShader(SHADER_PATH+"bezierShader", false);
    bf::GlfwStruct glfwStruct(settings,scene,deltaTime,io);
    glfwSetWindowUserPointer(window,&glfwStruct);

	scene.objectArray.add<bf::Torus>();
	bf::Point::initObjArrayRef(scene.objectArray);
	bf::BezierCommon::initData(scene, settings, window);

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
		if(ImGui::Button("Bézier curve inter")) {
			scene.objectArray.addRef<bf::BezierCurveInter>();
		}
		ImGui::End();
		//ImGui
		///LIST OF OBJECTS PANEL
		ImGui::Begin("List of objects");
		ImGui::Text("Hold CTRL and click to select multiple items.");
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
			if (scene.objectArray.imGuiCheckChanged(n, scene.multiCursor))
			{
                multiTransform = bf::Transform::Default;
				if (!settings.isCtrlPressed) { // Clear selection when CTRL is not held
					scene.objectArray.clearSelection(n);
				}
			}
		}
		ImGui::End();
		/// MODIFY OBJECT PANEL
		ImGui::Begin("Modify object");
		//active object settings
		if(scene.objectArray.getActiveIndex()!=-1 && !scene.objectArray.isMultipleActive())
			scene.objectArray[scene.objectArray.getActiveIndex()].ObjectGui();
		else {
			bool isAny = false;
			if (scene.objectArray.isMultipleActive()) {
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
		if(ImGui::Button("Save to file"))
			bf::saveToFile("../save.json",scene.objectArray);
		if(ImGui::Button("Load from file"))
			bf::loadFromFile("../save.json",scene.objectArray);
		ImGui::End();
		// Rendering
		ImGui::Render();
		//OpenGL draw
		scene.draw(shaderArray, settings);
		//end of OpenGL draw
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);

	}
	bf::imgui::destroy();
	bf::glfw::destroy(window);
	return 0;
}