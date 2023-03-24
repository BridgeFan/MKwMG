//
// Created by kamil-hp on 13.03.2022.
//
#include "ImGuiUtil.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <glm/glm.hpp>
#include <string>
#include <algorithm>
#include "misc/cpp/imgui_stdlib.h"
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif


ImGuiIO& initImGui(GLFWwindow* window) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
	return io;
}

void destroyImGui() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

bool checkChanged(const char* name, float& value) {
	float oldVal = value;
	ImGui::InputFloat(name,&value);
	return oldVal!=value;
}
bool checkChanged(const char* name, float& value, float min, float max, float step, float stepGreat) {
    float oldVal = value;
    ImGui::InputFloat(name,&value,step,stepGreat);
    value = std::clamp(value,min,max);
    return oldVal!=value;
}

bool checkChanged(const char* name, int& value) {
	int oldVal = value;
	ImGui::InputInt(name,&value);
	return oldVal!=value;
}
bool checkChanged(const char* name, int& value, int min, int max, int stepGreat) {
    int oldVal = value;
    ImGui::InputInt(name,&value,1,stepGreat);
    value = std::clamp(value,min,max);
    return oldVal!=value;
}

bool checkChanged(const char* name, std::string& value) {
	std::string oldVal = value;
	ImGui::InputText(name,&value);
    if(value.empty()) {
        value = oldVal;
    }
	return oldVal!=value;
}

bool checkChanged(const char* name, glm::vec3& values, bool isZeroInsurance) {
	float array[] = {values.x, values.y, values.z};
	ImGui::InputFloat3(name, array);
	bool ret = array[0]!=values.x || array[1]!=values.y || array[2]==values.z;
	values.x = isZeroInsurance && std::abs(array[0]) < 0.001f ? 0.001f * (array[0] >= 0.0f ? 1.f : -1.f) : array[0];
	values.y = isZeroInsurance && std::abs(array[1]) < 0.001f ? 0.001f * (array[1] >= 0.0f ? 1.f : -1.f) : array[1];
    values.z = isZeroInsurance && std::abs(array[2]) < 0.001f ? 0.001f * (array[2] >= 0.0f ? 1.f : -1.f) : array[2];
	return ret;
}

bool checkChanged(const char* name, glm::vec2& values) {
    float array[] = {values.x, values.y};
    ImGui::InputFloat2(name, array);
    bool ret = array[0]!=values.x || array[1]!=values.y;
    values.x = array[0];
    values.y = array[1];
    return ret;
}

bool checkSliderChanged(const char* name, int& value, int min, int max) {
	int oldVal = value;
	ImGui::SliderInt(name,&value, min, max);
    value=std::clamp(value,min,max);
	return oldVal!=value;
}

bool checkSliderChanged(const char* name, float& value, float min, float max) {
	float oldVal = value;
	ImGui::SliderFloat(name,&value, min, max);
    value=std::clamp(value,min,max);
	return oldVal!=value;
}

bool checkSelectableChanged(const char* name, bool& selectable) {
    bool wasChanged = ImGui::Selectable(name, selectable);
    if(wasChanged)
        selectable = !selectable;
    return wasChanged;
}

bool checkSelectableChanged(const char *name, std::vector<bool> &selectable, int n) {
    bool val = selectable[n];
    bool ret = checkSelectableChanged(name,val);
    if(ret)
        selectable[n]=!selectable[n];
    return ret;
}
