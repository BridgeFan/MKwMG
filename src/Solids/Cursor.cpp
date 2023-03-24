//
// Created by kamil-hp on 21.03.2022.
//

#include "Cursor.h"
#include "Shader.h"
#include "ImGuiUtil.h"
#include "imgui.h"
#include "Settings.h"
#include "GlfwUtil.h"
#include "Util.h"

void Cursor::draw(const Shader &shader, const Settings&) {
	for(auto & line : lines) {
			shader.setVec3("color",{1.0f,1.0f,.0f});
		line.setPosition(transform.position);
		line.setRotation(glm::vec3(.0f));
		line.setScale(glm::vec3(1.f));
		line.draw(shader);
	}
	shader.setVec3("color",1.f,1.f,1.f);
}

Cursor::Cursor(const Transform &transform) : lines{Solid(""),Solid(""),Solid("")},
transform(transform) {
    initLines();
}

void Cursor::initLines() {
	//set buffers
	for(int i=0;i<3;i++) {
		std::vector vec = {.0f,.0f,.0f, .0f, .0f, .0f};
        vec[i]=-.2f;
        vec[(i+1)%3]=-.2f;
		vec[3+i]=.2f;
        vec[3+(i+1)%3]=.2f;
		lines[i].vertices = std::move(vec);
		lines[i].indices = {0u,1u};
		lines[i].setBuffers();
	}
}

void Cursor::ObjectGui(GLFWwindow* window, const glm::mat4& view, const glm::mat4& inverseView, const glm::mat4& projection, const glm::mat4& inverseProjection) {
	ImGui::Text("Cursor");
	checkChanged("Cursor position", transform.position);
    if(window!=nullptr){
        glm::vec3 screenPos = toScreenPos(window, transform.position, view, projection);
        auto screenPos2 = glm::vec2(screenPos);
        if(checkChanged("Screen position", screenPos2)) {
            if(isnan(screenPos)) {
                screenPos2={.0f,.0f};
                screenPos.z=0.f;
            }
            transform.position = toGlobalPos(window, glm::vec3(screenPos2, screenPos.z), inverseView, inverseProjection);
        }
    }
}
