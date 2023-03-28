#include <GL/glew.h>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
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
#include "Solids/Cursor.h"
#include "Util.h"
#include "Solids/MultiCursor.h"
#include <memory>
#include <algorithm>
#include "src/Object/ObjectArray.h"
#include "Solids/BezierCurve.h"

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
	bf::ObjectArray objectArray;
    //std::vector<std::unique_ptr<bf::Object> > objects;
    bf::Cursor cursor;
    bf::MultiCursor multiCursor;
    bf::Transform& multiTransform=multiCursor.transform;
    glm::vec3 multiCentre;
    bf::Camera camera(0.1f,100.f,glm::vec3(0.0f, 0.0f, -10.0f),glm::vec3(0.0f,0.0f,0.f));
    float deltaTime = 0.0f;
    bf::GlfwStruct glfwStruct(settings,camera,/*selection,*/objectArray,deltaTime,io,cursor,multiCursor,multiTransform,multiCentre);
    glfwSetWindowUserPointer(window,&glfwStruct);
	//bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.25f, 0.25f, 0.20f, 1.00f);
	bf::Shader shader(SHADER_PATH+"shader.vert", SHADER_PATH+"shader.frag");

	objectArray.add<bf::Torus>();
	//objects.emplace_back(new bf::Torus());


    settings.Projection = bf::getProjectionMatrix(camera.Zoom,settings.aspect, camera.zNear, camera.zFar);
    settings.InverseProjection = bf::getInverseProjectionMatrix(camera.Zoom,settings.aspect, camera.zNear, camera.zFar);
    settings.View = camera.GetViewMatrix();
    settings.InverseView = camera.GetInverseViewMatrix();

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

		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
		///CREATE OBJECT PANEL
		ImGui::Begin("Create object");
		//ImGui::
		if(ImGui::Button("Torus")) {
			objectArray.add<bf::Torus>(cursor.transform);
			//objectArray.clearSelection(objectArray.size()-1, settings);
		}
		if(ImGui::Button("Point")) {
			objectArray.add<bf::Point>(cursor.transform);
			//objectArray.clearSelection(objectArray.size()-1, settings);
		}
		if(ImGui::Button("Bézier curve 0")) {
			objectArray.addRef<bf::BezierCurve>(camera, settings);
			//objectArray.clearSelection(objectArray.size()-1, settings);
		}
		ImGui::End();
		//ImGui
		///LIST OF OBJECTS PANEL
		ImGui::Begin("List of objects");
		/*if(selection.size()!=objectArray.size()) { //resize selection if needed
			selection.resize(objects.size(),false);
		}*/
		ImGui::Text("Hold CTRL and click to select multiple items.");
		if(ImGui::Button("Delete")) {
			objectArray.removeActive();
			//deleteObjects(selection, objects);
		}
		if(ImGui::Button(settings.isMultiState ? "To single" : "To multi")) {
			settings.isMultiState = !settings.isMultiState;
			if(!settings.isMultiState)
				objectArray.clearSelection(settings);
		}
        if(ImGui::Button(settings.isUniformScaling ? "To non-uniform scaling" : "To uniform scaling")) {
            settings.isUniformScaling = !settings.isUniformScaling;
        }
		for(int i=0;i<3;i++) {
			constexpr char axisName[] = {'X', 'Y', 'Z'};
			if(ImGui::Button((std::string((settings.isAxesLocked>>i)%2 ? "Unlock " : "Lock ")+axisName[i]).c_str())) {
				if ((settings.isAxesLocked >> i) % 2)
					settings.isAxesLocked -= 0x1 << i;
				else
					settings.isAxesLocked += 0x1 << i;
			}
		}

		if(ImGui::Button("Clear selection"))
			objectArray.clearSelection(-1, settings);
		for (std::size_t n = 0u; n < objectArray.size(); n++)
		{
			if(!objectArray.isCorrect(n)) {
				ImGui::Text("Empty unique pointer");
				continue;
			}
			if (bf::imgui::checkObjectArrayChanged(objectArray[n].name.c_str(), objectArray, n))
			{
                multiTransform = bf::Transform::Default;
				if (!settings.isMultiState) { // Clear selection when CTRL is not held
					objectArray.clearSelection(n, settings);
				}
				if(objectArray.isActive(n))
					settings.activeIndex = static_cast<int>(n);
				else
					settings.activeIndex = -1;
				multiCentre = objectArray.getCentre();
			}
		}
		ImGui::End();
		/// MODIFY OBJECT PANEL
		ImGui::Begin("Modify object");
		//active object settings
		if(settings.activeIndex>=0 && settings.activeIndex<static_cast<int>(objectArray.size()) && !settings.isMultiState)
			objectArray[settings.activeIndex].ObjectGui();
		else {
			bool isAny = false;
			if (settings.isMultiState) {
				for (std::size_t i = 0u; i < objectArray.size(); i++) {
					if (objectArray.isCorrect(i) && objectArray.isActive(i)) {
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
						for (std::size_t i = 0; i < objectArray.size(); i++) {
							if (objectArray.isCorrect(i) && objectArray.isActive(i)) {
								objectArray[i].setNewTransform(multiCentre, oldTransform, multiTransform);
							}
						}
					}
				}
			}
			if(!isAny) {
				cursor.ObjectGui(window, settings.View, settings.InverseView, settings.Projection, settings.InverseProjection);
			}
		}
		ImGui::End();
		//camera info and set cursor
		///CAMERA INFO PANEL
		ImGui::Begin("Camera info");
		camera.ObjectGui();
		ImGui::End();
		// Rendering
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		shader.use();
		// pass projection matrix to shader (note that in this case it could change every frame)
		settings.Projection = bf::getProjectionMatrix(camera.Zoom,settings.aspect, camera.zNear, camera.zFar);
		settings.InverseProjection = bf::getInverseProjectionMatrix(camera.Zoom,settings.aspect, camera.zNear, camera.zFar);
		shader.setMat4("projection", settings.Projection);
		// camera/view transformation
		settings.View = camera.GetViewMatrix();
		settings.InverseView = camera.GetInverseViewMatrix();
		shader.setMat4("view", settings.View);
		//draw objects
		for(std::size_t i=0;i<objectArray.size();i++) {
			if(objectArray.isCorrect(i)) {
				if(objectArray.isActive(i))
					shader.setVec3("color",1.f,.5f,.0f);
				else
					shader.setVec3("color",1.f,1.f,1.f);
				objectArray[i].draw(shader);
			}
		}
		if(settings.isMultiState && objectArray.isAnyActive()) {
            multiCursor.transform.position+=multiCentre;
            multiCursor.draw(shader, settings);
            multiCursor.transform.position-=multiCentre;
		}
        else if(!settings.isMultiState && objectArray.isMovable(settings.activeIndex)) {
            bf::Transform oldTransform = multiCursor.transform;
            multiCursor.transform=objectArray[settings.activeIndex].getTransform();
            multiCursor.draw(shader, settings);
            multiCursor.transform=std::move(oldTransform);
        }
        cursor.draw(shader, settings);
		//cursor.transform = cursorTransform;
		//end of OpenGL draw
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);

	}
	bf::imgui::destroy();
	bf::glfw::destroy(window);
	return 0;
}