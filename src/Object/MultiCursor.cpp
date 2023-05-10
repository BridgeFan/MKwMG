//
// Created by kamil-hp on 23.03.23.
//

#include "MultiCursor.h"
#include "src/Shader/ShaderArray.h"
#include "src/ImGui/ImGuiUtil.h"
#include "ConfigState.h"
#include "ImGui/imgui_include.h"

constexpr glm::vec3 multiColor[3] = {{1.f,0.f,0.f},{0.f,1.f,0.f},{0.f,0.f,1.f}};

void bf::MultiCursor::draw(const bf::ShaderArray &shaderArray, const bf::ConfigState& configState) {
    if(shaderArray.getActiveIndex()!=bf::ShaderType::BasicShader)
        return;
    for(int i=0;i<3;i++) {
        auto& line = lines[i];
        if((configState.isAxesLocked>>i)%2)
			shaderArray.setColor({0.f,.0f,.0f});
        else
			shaderArray.setColor(multiColor[i]);
        line.setPosition(transform.position);
        line.setRotation(transform.rotation);
        line.setScale(transform.scale);
        line.draw(shaderArray);
    }
	shaderArray.setColor(255,255,255);
}

bf::MultiCursor::MultiCursor(const bf::Transform &t) : lines{DummySolid(""),DummySolid(""),DummySolid("")},
                                                           transform(t) {
    initLines();
}

void bf::MultiCursor::initLines() {
    //set buffers
    for(int i=0;i<3;i++) {
        std::vector vec = {.0f, .0f, .0f};
        vec[i]=1.f;
        lines[i].vertices.emplace_back(.0f,.0f,.0f);
        lines[i].vertices.emplace_back(vec[0],vec[1],vec[2]);
        lines[i].indices = {0u,1u};
        lines[i].setBuffers();
    }
}

void bf::MultiCursor::ObjectGui() {
    ImGui::Text("Cursor");
	bf::imgui::checkChanged("Cursor position", transform.position);
	bf::imgui::checkChanged("Cursor rotation", transform.rotation);
	bf::imgui::checkChanged("Cursor scale", transform.scale, true);
}
