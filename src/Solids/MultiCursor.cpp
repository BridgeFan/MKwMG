//
// Created by kamil-hp on 23.03.23.
//

#include "MultiCursor.h"
#include "Shader.h"
#include "ImGuiUtil.h"
#include "Settings.h"
#include "imgui.h"

constexpr glm::vec3 multiColor[3] = {{1.f,0.f,0.f},{0.f,1.f,0.f},{0.f,0.f,1.f}};

void MultiCursor::draw(const Shader &shader, const Settings& settings) {
    for(int i=0;i<3;i++) {
        auto& line = lines[i];
        if((settings.isAxesLocked>>i)%2)
            shader.setVec3("color", {.0f,.0f,.0f});
        else
            shader.setVec3("color",multiColor[i]);
        line.setPosition(transform.position);
        line.setRotation(transform.rotation);
        line.setScale(transform.scale);
        line.draw(shader);
    }
    shader.setVec3("color",1.f,1.f,1.f);
}

MultiCursor::MultiCursor(const Transform &transform) : lines{Solid(""),Solid(""),Solid("")},
                                                           transform(transform) {
    initLines();
}

void MultiCursor::initLines() {
    //set buffers
    for(int i=0;i<3;i++) {
        std::vector vec = {.0f,.0f,.0f, .0f, .0f, .0f};
        vec[3+i]=1.f;
        lines[i].vertices = std::move(vec);
        lines[i].indices = {0u,1u};
        lines[i].setBuffers();
    }
}

void MultiCursor::ObjectGui() {
    ImGui::Text("Cursor");
    checkChanged("Cursor position", transform.position);
    checkChanged("Cursor rotation", transform.rotation);
    checkChanged("Cursor scale", transform.scale, true);
}
