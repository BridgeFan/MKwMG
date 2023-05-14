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

enum class SpecialPanel: short {
    None,
    FileLoadSavePanel,
    FileSaveMakeSurePanel,
    FileFailPanel
};
SpecialPanel activeSpecialPanel = SpecialPanel::None;

void bf::imgui::createObjectPanel(Scene &scene) {
    ImGui::Begin("Create object");
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
    if(activeSpecialPanel!=SpecialPanel::None)
        ImGui::EndDisabled();
    ImGui::End();
}

void bf::imgui::listOfObjectsPanel(bf::Scene &scene, bf::ConfigState& configState) {
    ImGui::Begin("List of objects");
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
    if(ImGui::Button("Delete")) {
        scene.objectArray.removeActive();
    }
    if(ImGui::Button("Clear selection"))
        scene.objectArray.clearSelection(-1);
    for (std::size_t n = 0u; n < scene.objectArray.size(); n++)
    {
        if(!scene.objectArray.isCorrect(n)) {
            ImGui::Text("Empty unique pointer");
            continue;
        }
		if(scene.objectArray[n].indestructibilityIndex>0)
			ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255,255,0,255));
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
    ImGui::End();
}

void bf::imgui::modifyObjectPanel(bf::Scene &scene, const bf::ConfigState& configState) {
    ImGui::Begin("Modify object panel");
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

int getTypeValue(const std::filesystem::path& p) {
    if(is_directory(p))
        return 0;
    else if(is_regular_file(p))
        return 2;
    else
        return 1;
}

void initPath(std::filesystem::path& path, std::vector<std::filesystem::path>& files) {
    std::filesystem::directory_iterator b(path), e;
    std::vector<std::filesystem::path> paths(b, e);
    files = std::move(paths);
    std::sort(files.begin(), files.end(), [](const auto& a, const auto& b) {
        return getTypeValue(a)<getTypeValue(b);
    });
}

void changePath(std::filesystem::path& path, std::vector<std::filesystem::path>& files, const std::filesystem::path *change) {
    if(!change && path.has_parent_path()) {
        path = path.parent_path();
    }
    else if(change){
        path = *change;
    }
    initPath(path, files);
}

void bf::imgui::cameraInfoPanel(bf::Scene &scene, bf::ConfigState& configState) {
    static bool isLoading = false;
    static bool wasPathChosen = false;
    static std::string name = "";
    static auto path = std::filesystem::current_path();
    static std::vector<std::filesystem::path> files;
    ImGui::Begin("Camera info");
    if(activeSpecialPanel!=SpecialPanel::None)
        ImGui::BeginDisabled();
    scene.camera.ObjectGui(configState);
    if(activeSpecialPanel!=SpecialPanel::None)
        ImGui::EndDisabled();
    if(activeSpecialPanel==SpecialPanel::None) {
        if (ImGui::Button("Save to file")) {
            isLoading = false;
            activeSpecialPanel = SpecialPanel::FileLoadSavePanel;
            initPath(path,files);
            name = "";
        }
        ImGui::SameLine();
        if (ImGui::Button("Load from file")) {
            isLoading = true;
            activeSpecialPanel = SpecialPanel::FileLoadSavePanel;
            initPath(path,files);
            wasPathChosen = false;
        }
    }
    ImGui::End();
    if(activeSpecialPanel==SpecialPanel::FileLoadSavePanel) {
        ImGui::Begin("Choose file");
        ImGui::Text("%s", path.c_str());
        if(ImGui::Selectable(".."))
            changePath(path,files,nullptr);
        for (const auto & entry : files) {
            if(is_directory(entry)) {
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255,127,0,255));
                if(ImGui::Selectable(entry.filename().c_str())) {
                    changePath(path,files,&entry);
                    ImGui::PopStyleColor();
                    break;
                }
                ImGui::PopStyleColor();
            }
            else if(entry.extension()==".json"){
                if(ImGui::Selectable(entry.filename().c_str())) {
                    auto tmp = entry.filename();
                    if(!isLoading)
                        tmp.replace_extension("");
                    name = tmp.string();
                    wasPathChosen=true;
                    break;
                }
            }
        }
        if(isLoading) {
            ImGui::Text("Chosen path: %s", (path.string() + "/" + name).c_str());
        }
        else {
            if(bf::imgui::checkChanged("File path (+.json)", name))
                wasPathChosen = !name.empty();
        }
        if(!wasPathChosen)
            ImGui::BeginDisabled();
        if(ImGui::Button(isLoading ? "Load" : "Save")) {
            if(isLoading) {
                if(!bf::loadFromFile(scene.objectArray, path.string()+"/"+name))
                    activeSpecialPanel = SpecialPanel::FileFailPanel;
                else
                    activeSpecialPanel = SpecialPanel::None;
            }
            else {
                if(std::filesystem::exists(path.string() + "/" + name + ".json"))
                    activeSpecialPanel = SpecialPanel::FileSaveMakeSurePanel;
                else if (bf::saveToFile(scene.objectArray, path.string() + "/" + name + ".json"))
                    activeSpecialPanel = SpecialPanel::FileFailPanel;
                else
                    activeSpecialPanel = SpecialPanel::None;
            }
        }
        if(!wasPathChosen)
            ImGui::EndDisabled();
        if(ImGui::Button("Cancel"))
            activeSpecialPanel=SpecialPanel::None;
        ImGui::End();
    }
    else if(activeSpecialPanel==SpecialPanel::FileSaveMakeSurePanel) {
        ImGui::Begin("File2");
        std::string makeSureText = std::format("Are you sure you want to override file {}?", name + ".json");
        ImGui::Text("%s", makeSureText.c_str());
        if(ImGui::Button("Yes")) {
            if (bf::saveToFile(scene.objectArray, path.string() + "/" + name + ".json"))
                activeSpecialPanel = SpecialPanel::FileFailPanel;
            else
                activeSpecialPanel = SpecialPanel::None;
        }
        ImGui::SameLine();
        if(ImGui::Button("No"))
            activeSpecialPanel=SpecialPanel::None;
        ImGui::End();

    }
    else if(activeSpecialPanel==SpecialPanel::FileFailPanel) {
        ImGui::Begin("File2");
        std::string failText = std::format("Failed to {} file {}!", isLoading ? "load" : "save", name);
		ImGui::TextColored({255,0,0,255},"%s", failText.c_str());
        if(ImGui::Button("OK"))
            activeSpecialPanel=SpecialPanel::None;
        ImGui::End();
    }
}

