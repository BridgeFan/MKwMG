//
// Created by kamil-hp on 23.03.23.
//

#include "MultiCursor.h"
#include "ShaderArray.h"
#include "src/ImGui/ImGuiUtil.h"
#include "ConfigState.h"
#include "imgui-master/imgui.h"

constexpr glm::vec3 multiColor[3] = {{1.f,0.f,0.f},{0.f,1.f,0.f},{0.f,0.f,1.f}};

void bf::MultiCursor::draw(const bf::ShaderArray &shaderArray, const bf::ConfigState& configState) {
    if(shaderArray.getActiveIndex()!=bf::ShaderType::BasicShader)
        return;
    const auto& shader = shaderArray.getActiveShader();
    for(int i=0;i<3;i++) {
        auto& line = lines[i];
        if((configState.isAxesLocked>>i)%2)
            shader.setVec3("color", {.0f,.0f,.0f});
        else
            shader.setVec3("color",multiColor[i]);
        line.setPosition(transform.position);
        line.setRotation(transform.rotation);
        line.setScale(transform.scale);
        line.draw(shaderArray);
    }
    shader.setVec3("color",1.f,1.f,1.f);
}

bf::MultiCursor::MultiCursor(const bf::Transform &t) : lines{Solid(""),Solid(""),Solid("")},
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
