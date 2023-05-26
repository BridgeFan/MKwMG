//
// Created by kamil-hp on 21.03.2022.
//
#include "ImGui/imgui_include.h"
#include "Cursor.h"
#include "Shader/ShaderArray.h"
#include "ImGui/ImGuiUtil.h"
#include "Util.h"
#include "ConfigState.h"

void bf::Cursor::draw(const bf::ShaderArray &shaderArray, const bf::ConfigState& configState, const glm::vec3& cameraPosition) {
    if(shaderArray.getActiveIndex()!=bf::ShaderType::CursorShader) {
		return;
	}
    shaderArray.getActiveShader().setFloat("cameraDistance", glm::distance(cameraPosition, transform.position));
	for(auto & line : lines) {
		shaderArray.setColor(255,255,0);
		line.setPosition(transform.position);
		line.setRotation(glm::vec3(.0f));
		line.setScale(glm::vec3(configState.gizmoSize));
		line.anyDraw(shaderArray);
	}
	shaderArray.setColor(255,255,255);
}

bf::Cursor::Cursor(const Transform &t) : lines{DummySolid(""),DummySolid(""),DummySolid("")},
transform(t) {
    initLines();
}

void bf::Cursor::initLines() {
	//set buffers
	for(unsigned i=0u;i<3u;i++) {
		std::vector vec = {.0f,.0f,.0f};
        vec[i]=.03f;
        vec[(i+1)%3]=.03f;
		lines[i].vertices.emplace_back(-vec[0],-vec[1],-vec[2]);
        lines[i].vertices.emplace_back(vec[0],vec[1],vec[2]);
		lines[i].indices = {0u,1u};
		lines[i].setBuffers();
	}
}

void bf::Cursor::ObjectGui(int screenWidth, int screenHeight, const glm::mat4& view, const glm::mat4& inverseView, const glm::mat4& projection, const glm::mat4& inverseProjection) {
	ImGui::Text("Cursor");
	bf::imgui::checkChanged("Cursor position", transform.position);
    {
        glm::vec3 screenPos = bf::toScreenPos(screenWidth, screenHeight, transform.position, view, projection);
        auto screenPos2 = glm::vec2(screenPos);
        if(bf::isInBounds(screenWidth, screenHeight, screenPos2)) {
            if (bf::imgui::checkChanged("Screen position", screenPos2)) {
                if (isnan(screenPos)) {
                    screenPos2 = {.0f, .0f};
                    screenPos.z = 0.f;
                }
                transform.position = bf::toGlobalPos(screenWidth, screenHeight, glm::vec3(screenPos2, screenPos.z), inverseView,
                                                           inverseProjection);
            }
        }
        else {
            ImGui::Text("Screen position");
        }
    }
}
