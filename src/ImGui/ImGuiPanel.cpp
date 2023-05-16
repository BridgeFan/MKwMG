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

ImU32 ImCol32(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
#pragma GCC diagnostic ignored "-Wold-style-cast"
    return IM_COL32(r,g,b,a);
#pragma GCC diagnostic warning "-Wold-style-cast"
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
    if(ImGui::Button("TODO: BezSuf2")) {
        //TODO: Bézier surface 2
    }
    if(activeSpecialPanel!=SpecialPanel::None)
        ImGui::EndDisabled();
    ImGui::End();
}

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

int getTypeValue(const std::filesystem::path& p) {
    if(p.empty())
        return INT_MAX;
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
    int countBad=0;
    for(auto& subpath: files) {
        if(is_directory(subpath)) {
            try{
                std::filesystem::directory_iterator c(subpath);
            }
            catch(...) {
                countBad++;
                subpath=std::filesystem::path();
            }
        }
    }
    std::sort(files.begin(), files.end(), [&](const auto& a, const auto& b2) {
        if(getTypeValue(a)!=getTypeValue(b2))
            return getTypeValue(a)<getTypeValue(b2);
        return a.string()<b2.string();
    });
    for(int i=0;i<countBad;i++)
        files.pop_back();
}

void changePath(std::filesystem::path& path, std::vector<std::filesystem::path>& files, const std::filesystem::path *change) {
    auto oldPath = path;
    if(!change && path.has_parent_path()) {
        path = path.parent_path();
    }
    else if(change){
        path = *change;
    }
    try {
        initPath(path, files);
    }
    catch(...) {
        std::cerr << "File cannot be read\n";
        path=oldPath;
    }
}

static std::string name = "";
static auto path = std::filesystem::current_path();
static auto namePath = std::filesystem::current_path();
static std::vector<std::filesystem::path> files;

void fileLoadSavePanel(bool isLoading, bf::Scene& scene, const bf::ConfigState& cs, bool& wasPathChosen) {
    setNextPanelAlignment({350, 280}, {cs.screenWidth, cs.screenHeight}, {.5f, .5f});
    ImGui::SetNextWindowSize(ImVec2(350, 280));
    ImGui::Begin("Choose file", nullptr, ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoCollapse);
    ImGui::Text("%s", path.c_str());
    ImGui::BeginChild("Objects", ImVec2(ImGui::GetContentRegionAvail().x, 180), true);
    if(ImGui::Selectable(".."))
        changePath(path,files,nullptr);
    for (const auto & entry : files) {
        if(is_directory(entry)) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImCol32(255,127,0,255));
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
                namePath = path;
                wasPathChosen=true;
                break;
            }
        }
    }
    ImGui::EndChild();
    if(isLoading) {
        auto txtPath = std::filesystem::relative(namePath, path);
        ImGui::Text("Chosen path: %s", (txtPath.string() + "/" + name).c_str());
    }
    else {
        if(bf::imgui::checkChanged("File path (+.json)", name))
            wasPathChosen = !name.empty();
    }
    if(!wasPathChosen)
        ImGui::BeginDisabled();
    if(ImGui::Button(isLoading ? "Load" : "Save")) {
        if(isLoading) {
            if(!bf::loadFromFile(scene.objectArray, namePath.string()+"/"+name))
                activeSpecialPanel = SpecialPanel::FileFailPanel;
            else
                activeSpecialPanel = SpecialPanel::None;
        }
        else {
            if(std::filesystem::exists(path.string() + "/" + name + ".json"))
                activeSpecialPanel = SpecialPanel::FileSaveMakeSurePanel;
            else if (bf::saveToFile(scene.objectArray, path.string() + "/" + name + ".json"))
                activeSpecialPanel = SpecialPanel::None;
            else
                activeSpecialPanel = SpecialPanel::FileFailPanel;
        }
    }
    if(!wasPathChosen)
        ImGui::EndDisabled();
    ImGui::SameLine();
    if(ImGui::Button("Cancel"))
        activeSpecialPanel=SpecialPanel::None;
    ImGui::End();
}

void bf::imgui::cameraInfoPanel(bf::Scene &scene, bf::ConfigState& configState) {
    static bool isLoading = false;
    static bool wasPathChosen = false;
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
        fileLoadSavePanel(isLoading, scene, configState, wasPathChosen);
    }
    else if(activeSpecialPanel==SpecialPanel::FileSaveMakeSurePanel) {
        setNextPanelAlignment({450, 75}, {configState.screenWidth, configState.screenHeight}, {.5f,.5f});
        ImGui::Begin("File2", nullptr, ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoCollapse);
        std::string makeSureText = std::format("Are you sure you want to override file {}?", name + ".json");
        ImGui::Text("%s", makeSureText.c_str());
        if(ImGui::Button("Yes")) {
            if (bf::saveToFile(scene.objectArray, path.string() + "/" + name + ".json"))
                activeSpecialPanel = SpecialPanel::None;
            else
                activeSpecialPanel = SpecialPanel::FileFailPanel;
        }
        ImGui::SameLine();
        if(ImGui::Button("No"))
            activeSpecialPanel=SpecialPanel::None;
        ImGui::End();

    }
    else if(activeSpecialPanel==SpecialPanel::FileFailPanel) {
        setNextPanelAlignment({350, 75}, {configState.screenWidth, configState.screenHeight}, {.5f,.5f});
        ImGui::Begin("File2", nullptr, ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoCollapse);
        std::string failText = std::format("Failed to {} file {}!", isLoading ? "load" : "save", name);
		ImGui::TextColored({255,0,0,255},"%s", failText.c_str());
        if(ImGui::Button("OK"))
            activeSpecialPanel=SpecialPanel::None;
        ImGui::End();
    }
}

