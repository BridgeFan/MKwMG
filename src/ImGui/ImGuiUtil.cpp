//
// Created by kamil-hp on 13.03.2022.
//
#include "ImGuiUtil.h"
#define IMGUI_BACKEND
#include "imgui_include.h"
#include <string>
#include <algorithm>
#include <format>
#include "Util.h"

#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif


ImGuiIO& init(GLFWwindow* window) {
	//IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 420 core");
	return io;
}

void destroy() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

bool bf::imgui::checkChanged(const char* name, float& value) {
	float oldVal = value;
	ImGui::InputFloat(name,&value);
	return !almostEqual(oldVal,value);
}
bool bf::imgui::checkChanged(const char* name, float& value, float min, float max, float step, float stepGreat) {
    float oldVal = value;
    ImGui::InputFloat(name,&value,step,stepGreat);
    value = std::clamp(value,min,max);
	return !almostEqual(oldVal,value);
}

bool bf::imgui::checkChanged(const char* name, int& value) {
	int oldVal = value;
	ImGui::InputInt(name,&value);
	return oldVal!=value;
}
bool bf::imgui::checkChanged(const char* name, int& value, int min, int max, int stepGreat) {
    int oldVal = value;
    ImGui::InputInt(name,&value,1,stepGreat);
    value = std::clamp(value,min,max);
    return oldVal!=value;
}

bool bf::imgui::checkChanged(const char* name, std::string& value) {
	std::string oldVal = value;
	ImGui::InputText(name,&value);
    if(value.empty()) {
        value = oldVal;
    }
	return oldVal!=value;
}

bool bf::imgui::checkChanged(const char* name, glm::vec3& values, bool isZeroInsurance) {
	float array[] = {values.x, values.y, values.z};
	ImGui::InputFloat3(name, array);
	bool ret = !almostEqual(array[0],values.x) || !almostEqual(array[1],values.y) || !almostEqual(array[2],values.z);
	values.x = isZeroInsurance && std::abs(array[0]) < 0.001f ? 0.001f * (array[0] >= 0.0f ? 1.f : -1.f) : array[0];
	values.y = isZeroInsurance && std::abs(array[1]) < 0.001f ? 0.001f * (array[1] >= 0.0f ? 1.f : -1.f) : array[1];
    values.z = isZeroInsurance && std::abs(array[2]) < 0.001f ? 0.001f * (array[2] >= 0.0f ? 1.f : -1.f) : array[2];
	return ret;
}

bool bf::imgui::checkChanged(const char* name, glm::vec2& values, const char* format) {
    float array[] = {values.x, values.y};
	if(format)
    	ImGui::InputFloat2(name, array, format);
	else
		ImGui::InputFloat2(name, array);
    bool ret = !almostEqual(array[0],values.x, 1e-4) || !almostEqual(array[1],values.y, 1e-4);
    if(!ret)
        return false;
    values.x = array[0];
    values.y = array[1];
    return true;
}
bool bf::imgui::checkChanged(const char* name, glm::vec<2,int>& values) {
	int array[] = {values.x, values.y};
	bool ret = ImGui::InputInt2(name, array);
    values = {array[0], array[1]};
    return ret;
}

bool bf::imgui::checkSliderChanged(const char* name, int& value, int min, int max) {
	int oldVal = value;
	ImGui::SliderInt(name,&value, min, max);
    value=std::clamp(value,min,max);
	return oldVal!=value;
}

bool bf::imgui::checkSliderChanged(const char* name, float& value, float min, float max) {
	float oldVal = value;
	ImGui::SliderFloat(name,&value, min, max);
    value=std::clamp(value,min,max);
	return !almostEqual(oldVal,value);
}

bool bf::imgui::checkSelectableChanged(const char* name, int index, bool& selectable) {
    bool wasChanged = ImGui::Selectable(std::format("{0}##{1}", name, index).c_str(), selectable);
    if(wasChanged)
        selectable = !selectable;
    return wasChanged;
}

void bf::imgui::preDraw() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void bf::imgui::postDraw() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

bf::imgui::IO::IO(GLFWwindow *window) : io(init(window)) {}

bf::imgui::IO::~IO() {
    destroy();
}
