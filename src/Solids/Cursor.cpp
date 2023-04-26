//
// Created by kamil-hp on 21.03.2022.
//
#include "imgui-master/imgui.h"
#include "Cursor.h"
#include "ShaderArray.h"
#include "ImGuiUtil.h"
#include "GlfwUtil.h"
#include "Util.h"

void bf::Cursor::draw(const bf::ShaderArray &shaderArray) {
    if(shaderArray.getActiveIndex()!=bf::ShaderType::BasicShader)
        return;
    const auto& shader = shaderArray.getActiveShader();
	for(auto & line : lines) {
			shader.setVec3("color",{1.0f,1.0f,.0f});
		line.setPosition(transform.position);
		line.setRotation(glm::vec3(.0f));
		line.setScale(glm::vec3(1.f));
		line.draw(shaderArray);
	}
	shader.setVec3("color",1.f,1.f,1.f);
}

bf::Cursor::Cursor(const Transform &t) : lines{Solid(""),Solid(""),Solid("")},
transform(t) {
    initLines();
}

void bf::Cursor::initLines() {
	//set buffers
	for(unsigned i=0u;i<3u;i++) {
		std::vector vec = {.0f,.0f,.0f};
        vec[i]=.2f;
        vec[(i+1)%3]=.2f;
		lines[i].vertices.emplace_back(-vec[0],-vec[1],-vec[2]);
        lines[i].vertices.emplace_back(vec[0],vec[1],vec[2]);
		lines[i].indices = {0u,1u};
		lines[i].setBuffers();
	}
}

void bf::Cursor::ObjectGui(GLFWwindow* window, const glm::mat4& view, const glm::mat4& inverseView, const glm::mat4& projection, const glm::mat4& inverseProjection) {
	ImGui::Text("Cursor");
	bf::imgui::checkChanged("Cursor position", transform.position);
    if(window!=nullptr){
        glm::vec3 screenPos = bf::glfw::toScreenPos(window, transform.position, view, projection);
        auto screenPos2 = glm::vec2(screenPos);
        if(bf::glfw::isInBounds(window, screenPos2)) {
            if (bf::imgui::checkChanged("Screen position", screenPos2)) {
                if (isnan(screenPos)) {
                    screenPos2 = {.0f, .0f};
                    screenPos.z = 0.f;
                }
                transform.position = bf::glfw::toGlobalPos(window, glm::vec3(screenPos2, screenPos.z), inverseView,
                                                           inverseProjection);
            }
        }
        else {
            ImGui::Text("Screen position");
        }
    }
}
