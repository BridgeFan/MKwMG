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

const std::string SHADER_PATH = "../shaders/";

int main() {
    Settings settings;
	GLFWwindow* window=initGlfw(settings);
	if(!window) {

        fprintf(stderr, "Fatal error creating GLFW window\n");
		return EXIT_FAILURE;
	}
	//init ImGUI
    //structures
	ImGuiIO& io = initImGui(window);
    std::vector<std::unique_ptr<Object> > objects;
    Cursor cursor;
    MultiCursor multiCursor;
    Transform& multiTransform=multiCursor.transform;
    glm::vec3 multiCentre;
    Camera camera(glm::vec3(0.0f, 0.0f, -10.0f),glm::vec3(0.0f,0.0f,0.f));
    std::vector<bool> selection;
    float deltaTime = 0.0f;
    GlfwStruct glfwStruct(settings,camera,selection,objects,deltaTime,io,cursor,multiCursor,multiTransform,multiCentre);
    glfwSetWindowUserPointer(window,&glfwStruct);


	//bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.25f, 0.25f, 0.20f, 1.00f);

	Shader shader(SHADER_PATH+"shader.vert", SHADER_PATH+"shader.frag");

	objects.emplace_back(new Torus());

    Transform myTransform{{1.f,-1.f,2.f},{30.f,45.f,55.f},{.8f,1.15f,1.2f}};


    settings.Projection = getProjectionMatrix(camera.Zoom,settings.aspect, 0.1f, 100.f);
    settings.InverseProjection = getInverseProjectionMatrix(camera.Zoom,settings.aspect, 0.1f, 100.f);
    settings.View = camera.GetViewMatrix();
    settings.InverseView = camera.GetInverseViewMatrix();

	while (!glfwWindowShouldClose(window))
	{
		static float lastFrame;
		auto currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		// Poll and handle events (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		glfwPollEvents();
		processInput(window, camera, io);

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
		///CREATE OBJECT PANEL
		ImGui::Begin("Create object");
		//ImGui::
		if(ImGui::Button("Torus")) {
			objects.emplace_back(new Torus(cursor.transform));
			clearSelection(selection,-1,settings);
			settings.activeIndex = selection.size();
			selection.emplace_back(true);
		}
		if(ImGui::Button("Point")) {
			objects.emplace_back(new Point(cursor.transform));
			clearSelection(selection,-1,settings);
			settings.activeIndex = selection.size();
			selection.emplace_back(true);
		}
		ImGui::End();
		//ImGui
		///LIST OF OBJECTS PANEL
		ImGui::Begin("List of objects");
		if(selection.size()!=objects.size()) { //resize selection if needed
			selection.resize(objects.size(),false);
		}
		ImGui::Text("Hold CTRL and click to select multiple items.");
		if(ImGui::Button("Delete")) {
			deleteObjects(selection, objects);
		}
		if(ImGui::Button(settings.isMultiState ? "To single" : "To multi")) {
			settings.isMultiState = !settings.isMultiState;
			if(!settings.isMultiState)
				clearSelection(selection, -1, settings);
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
			clearSelection(selection, -1, settings);
		for (int n = 0; n < (int)selection.size(); n++)
		{
			if(!objects[n]) {
				ImGui::Text("Empty unique pointer");
				continue;
			}
			if (checkSelectableChanged(objects[n]->name.c_str(), selection, n))
			{
                multiTransform = Transform::Default;
				if (!settings.isMultiState) { // Clear selection when CTRL is not held
					clearSelection(selection, n, settings);
				}
				//selection[n] = !selection[n];
				if(selection[n])
					settings.activeIndex = n;
				else
					settings.activeIndex = -1;
				multiCentre = getCentre(selection, objects);
			}
		}
		ImGui::End();
		/// MODIFY OBJECT PANEL
		ImGui::Begin("Modify object");
		//active object settings
		if(settings.activeIndex>=0 && settings.activeIndex<(int)objects.size() && !settings.isMultiState)
			objects[settings.activeIndex]->ObjectGui();
		else {
			bool isAny = false;
			if (settings.isMultiState) {
				for (int i = 0; i < (int)objects.size(); i++) {
					if (objects[i] && selection[i]) {
						isAny=true;
						break;
					}
				}
				if(isAny) {
					ImGui::Text("Multiple objects");
					Transform oldTransform = multiTransform;
					bool tmp = checkChanged("Object position", multiTransform.position);
					tmp = checkChanged("Object rotation", multiTransform.rotation) || tmp;
					tmp = checkChanged("Object scale", multiTransform.scale, true) || tmp;
					//multiCentre = getCentre(selection, objects);
					if (tmp) {
						for (int i = 0; i < (int)objects.size(); i++) {
							if (objects[i] && selection[i]) {
								objects[i]->setNewTransform(multiCentre, oldTransform, multiTransform);
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
        settings.Projection = getProjectionMatrix(camera.Zoom,settings.aspect, 0.1f, 100.f);
        settings.InverseProjection = getInverseProjectionMatrix(camera.Zoom,settings.aspect, 0.1f, 100.f);
		shader.setMat4("projection", settings.Projection);
		// camera/view transformation
		settings.View = camera.GetViewMatrix();
        settings.InverseView = camera.GetInverseViewMatrix();
		shader.setMat4("view", settings.View);
		//draw objects
		for(int i=0;i<(int)objects.size();i++) {
			if(objects[i]) {
				if(selection[i])
					shader.setVec3("color",1.f,.5f,.0f);
				else
					shader.setVec3("color",1.f,1.f,1.f);
				objects[i]->draw(shader);
			}
		}
		if(settings.isMultiState && std::find(selection.begin(),selection.end(),true)!=selection.end()) {
            multiCursor.transform.position+=multiCentre;
            multiCursor.draw(shader, settings);
            multiCursor.transform.position-=multiCentre;
		}
        else if(!settings.isMultiState && settings.activeIndex>=0 && settings.activeIndex < (int)selection.size() &&
            objects[settings.activeIndex]) {
            Transform oldTransform = multiCursor.transform;
            multiCursor.transform=objects[settings.activeIndex]->getTransform();
            multiCursor.draw(shader, settings);
            multiCursor.transform=std::move(oldTransform);
        }
        cursor.draw(shader, settings);
		//cursor.transform = cursorTransform;
		//end of OpenGL draw
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);

	}
	destroyImGui();
	destroyGlfw(window);
	return 0;
}