//
// Created by kamil-hp on 27.04.23.
//
#include <format>
#include <iostream>
#include <filesystem>
#include <algorithm>
#include "ImGuiUtil.h"
#include "ImGui/imgui_include.h"
#include "ImGui/ImGuiPanel.h"
#include "Scene.h"
#include "ConfigState.h"
#include "Solids/Torus.h"
#include "src/Object/Point.h"
#include "Curves/BezierCurve0.h"
#include "Curves/BezierCurve2.h"
#include "Curves/BezierCurveInter.h"
#include "FileLoading.h"
#include "Surfaces/BezierSurface0.h"
#include "Surfaces/BezierSurface2.h"
#include "lib/ImGui-Addons/FileBrowser/ImGuiFileBrowser.h"

enum class SpecialPanel: short {
    None,
    FileLoadSavePanel,
    FileFailPanel
};
SpecialPanel activeSpecialPanel = SpecialPanel::None;

ImU32 ImCol32(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
#ifdef GCC
#pragma GCC diagnostic ignored "-Wold-style-cast"
    return IM_COL32(r,g,b,a);
#pragma GCC diagnostic warning "-Wold-style-cast"
#else
    return IM_COL32(r, g, b, a);
#endif
}
ImVec2 toImgui(const glm::vec2& v) {
    return {v.x, v.y};
}

void setNextPanelAlignment(const glm::vec2& panelSize, const glm::vec2& screenSize, const glm::vec2& t, const glm::vec2& modPos={0,0}) {
    ImGui::SetNextWindowSize(toImgui(panelSize));
    ImGui::SetNextWindowPos(toImgui((screenSize-panelSize)*t+modPos));
}

void bf::imgui::createObjectPanel(Scene &scene, const bf::ConfigState& configState) {
    setNextPanelAlignment({155, 192}, {configState.screenWidth, configState.screenHeight}, {1.f,0.f},{0,175});
    ImGui::Begin("Create object",nullptr, ImGuiWindowFlags_NoResize);
    if(activeSpecialPanel!=SpecialPanel::None)
        ImGui::BeginDisabled();
    if(ImGui::Button("Torus")) {
        scene.objectArray.add<bf::Torus>(scene.cursor.transform);
    }
    if(ImGui::Button("Point")) {
        scene.objectArray.add<bf::Point>(scene.cursor.transform);
        int redIndex = scene.objectArray.getActiveRedirector();
        if(scene.objectArray.isCorrect(redIndex)) {
            scene.objectArray[redIndex].addPoint(scene.objectArray.size()-1);
        }
    }
    if(ImGui::Button("Bézier curve 0")) {
        scene.objectArray.addRef<bf::BezierCurve0>();
    }
    if(ImGui::Button("Bézier curve 2")) {
        scene.objectArray.addRef<bf::BezierCurve2>();
    }
    if(ImGui::Button("Bézier curve inter")) {
        scene.objectArray.addRef<bf::BezierCurveInter>();
    }
	if(ImGui::Button("Bézier surface 0")) {
		scene.objectArray.addRef<bf::BezierSurface0>(scene.cursor);
	}
    if(ImGui::Button("Bézier surface 2")) {
        scene.objectArray.addRef<bf::BezierSurface2>(scene.cursor);
    }
    if(activeSpecialPanel!=SpecialPanel::None)
        ImGui::EndDisabled();
    ImGui::End();
}

imgui_addons::ImGuiFileBrowser file_dialog;

void bf::imgui::listOfObjectsPanel(bf::Scene &scene, bf::ConfigState& configState) {
    setNextPanelAlignment({275, 520}, {configState.screenWidth, configState.screenHeight}, {0.f,.5f});
    ImGui::Begin("List of objects", nullptr, ImGuiWindowFlags_NoResize);
    if(activeSpecialPanel!=SpecialPanel::None)
        ImGui::BeginDisabled();
    ImGui::Text("Hold CTRL and click to select multiple items.");
    ImGui::Checkbox("Uniform scalng", &configState.isUniformScaling);
	ImGui::Checkbox("Stereoscopic", &configState.stereoscopic);
    if(configState.stereoscopic) {
        ImGui::SliderFloat("Intraocular distance", &configState.IOD, .0f, 1.f);
        bf::imgui::checkSliderChanged("Convergenece", configState.convergence, configState.cameraNear, configState.cameraFar);
    }
    else {
        ImGui::Text(U8("--------"));
        ImGui::Text(U8("--------"));
    }
    bf::imgui::checkSliderChanged("Point half size", configState.pointRadius, 1.f, 16.f);
    if(bf::imgui::checkSliderChanged("Gray percentage", configState.grayPercentage, .0f, 1.f)) {
        scene.shaderArray.setGrayPercentage(configState.grayPercentage);
    }
    ImGui::Text("Lock: ");
    for(int i=0;i<3;i++) {
        static bool tmp[3];
        static const std::string axisName[] = {"X", "Y", "Z"};
        tmp[i]=configState.isAxesLocked&(0x1<<i);
        ImGui::SameLine();
        if(ImGui::Checkbox(axisName[i].c_str(), &tmp[i])) {
            if (tmp[i])
                configState.isAxesLocked += 0x1 << i;
            else
                configState.isAxesLocked -= 0x1 << i;
        }
    }
    if(ImGui::Button("Delete selected")) {
        scene.objectArray.removeActive();
    }
    ImGui::SameLine();
    if(ImGui::Button("Clear selection"))
        scene.objectArray.clearSelection(-1);
    ImGui::BeginChild("Objects", ImVec2(ImGui::GetContentRegionAvail().x, 260), true);
    if(activeSpecialPanel!=SpecialPanel::None)
        ImGui::BeginDisabled();
    for (std::size_t n = 0u; n < scene.objectArray.size(); n++)
    {
        if(!scene.objectArray.isCorrect(n)) {
            ImGui::Text("Empty unique pointer");
            continue;
        }
		if(scene.objectArray[n].indestructibilityIndex>0)
			ImGui::PushStyleColor(ImGuiCol_Text, ImCol32(255,255,0,255));
        if (scene.objectArray.imGuiCheckChanged(n, scene.multiCursor))
        {
            scene.multiCursor.transform = bf::Transform::Default;
            if (!configState.isCtrlPressed) { // Clear selection when CTRL is not held
                scene.objectArray.clearSelection(n);
            }
        }
		if(scene.objectArray[n].indestructibilityIndex>0)
			ImGui::PopStyleColor();
    }
    if(activeSpecialPanel!=SpecialPanel::None)
        ImGui::EndDisabled();
    ImGui::EndChild();
    if(activeSpecialPanel!=SpecialPanel::None)
        ImGui::EndDisabled();
    ImGui::End();
}

void bf::imgui::modifyObjectPanel(bf::Scene &scene, const bf::ConfigState& configState) {
    setNextPanelAlignment({300, 305}, {configState.screenWidth, configState.screenHeight}, {1.f,1.f});
    ImGui::Begin("Modify object panel", nullptr, ImGuiWindowFlags_NoResize);
    if(activeSpecialPanel!=SpecialPanel::None)
        ImGui::BeginDisabled();
    if(scene.objectArray.getActiveIndex()!=-1 && !scene.objectArray.isMultipleActive())
        scene.objectArray[scene.objectArray.getActiveIndex()].ObjectGui();
    else {
        bool isAny = false;
        if (scene.objectArray.isMultipleActive()) {
            for (std::size_t i = 0u; i < scene.objectArray.size(); i++) {
                if (scene.objectArray.isCorrect(i) && scene.objectArray.isActive(i)) {
                    isAny=true;
                    break;
                }
            }
            if(isAny) {
                ImGui::Text("Multiple objects");
                bf::Transform oldTransform = scene.multiCursor.transform;
                bool tmp = bf::imgui::checkChanged("Object position", scene.multiCursor.transform.position);
                tmp = bf::imgui::checkChanged("Object rotation", scene.multiCursor.transform.rotation) || tmp;
                tmp = bf::imgui::checkChanged("Object scale", scene.multiCursor.transform.scale, true) || tmp;
                if (tmp) {
                    for (std::size_t i = 0; i < scene.objectArray.size(); i++) {
                        if (scene.objectArray.isCorrect(i) && scene.objectArray.isActive(i)) {
                            scene.objectArray[i].setNewTransform(scene.objectArray.getCentre(), oldTransform, scene.multiCursor.transform);
                        }
                    }
                }
            }
        }
        if(!isAny) {
            scene.cursor.ObjectGui(configState.screenWidth, configState.screenHeight, scene.getView(),
                                   scene.getInverseView(), scene.getProjection(), scene.getInverseProjection());
        }
    }
    if(activeSpecialPanel!=SpecialPanel::None)
        ImGui::EndDisabled();
    ImGui::End();
}

static std::string name = "";

void fileLoadSavePanel(bool isLoading, bf::Scene& scene) {
    ImVec2 size = {700, 310};
    if(isLoading)
        ImGui::OpenPopup("Open File");
    else
        ImGui::OpenPopup("Save File");
    if(isLoading) {
        if (file_dialog.showFileDialog("Open File", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, size,
                                       ".json")) {
            if(!bf::loadFromFile(scene.objectArray, file_dialog.selected_path)) {
                name = file_dialog.selected_path;
                ImGui::OpenPopup("File fail");
                activeSpecialPanel = SpecialPanel::FileFailPanel;
            }
            else
                activeSpecialPanel = SpecialPanel::None;
        }
        if(!ImGui::IsPopupOpen("Open File"))
            activeSpecialPanel = SpecialPanel::None;
    }
    else {
        if (file_dialog.showFileDialog("Save File", imgui_addons::ImGuiFileBrowser::DialogMode::SAVE, size,
                                       ".json")) {
            if(!bf::saveToFile(scene.objectArray, file_dialog.selected_path+file_dialog.ext)) {
                name = file_dialog.selected_path+file_dialog.ext;
                ImGui::OpenPopup("File fail");
                activeSpecialPanel = SpecialPanel::FileFailPanel;
            }
            else
                activeSpecialPanel = SpecialPanel::None;
        }
        if(!ImGui::IsPopupOpen("Save File"))
            activeSpecialPanel = SpecialPanel::None;
    }
    ImGui::CloseCurrentPopup();
}

void bf::imgui::cameraInfoPanel(bf::Scene &scene, bf::ConfigState& configState) {
    static bool isLoading = false;
    setNextPanelAlignment({275, 170}, {configState.screenWidth, configState.screenHeight}, {1.f,.0f});
    ImGui::Begin("Camera info", nullptr, ImGuiWindowFlags_NoResize);
    if(activeSpecialPanel!=SpecialPanel::None)
        ImGui::BeginDisabled();
    scene.camera.ObjectGui(configState);
    if(activeSpecialPanel!=SpecialPanel::None)
        ImGui::EndDisabled();
    if(activeSpecialPanel==SpecialPanel::None) {
        if (ImGui::Button("Save to file")) {
            isLoading = false;
            activeSpecialPanel = SpecialPanel::FileLoadSavePanel;
        }
        ImGui::SameLine();
        if (ImGui::Button("Load from file")) {
            isLoading = true;
            activeSpecialPanel = SpecialPanel::FileLoadSavePanel;
        }
    }
    ImGui::End();
    if(activeSpecialPanel==SpecialPanel::FileLoadSavePanel) {
        fileLoadSavePanel(isLoading, scene);
    }
    if(ImGui::IsPopupOpen("File fail")) {
        setNextPanelAlignment({450, 75}, {configState.screenWidth, configState.screenHeight}, {.5f, .5f});
    }
    if(ImGui::BeginPopupModal("File fail")) {
        std::string failText = std::format("Failed to {} file {}!", isLoading ? "load" : "save", name);
		ImGui::TextColored({255,0,0,255},"%s", failText.c_str());
        if(ImGui::Button("OK")) {
            ImGui::CloseCurrentPopup();
            activeSpecialPanel=SpecialPanel::None;
        }
        ImGui::EndPopup();
    }
}

