//
// Created by kamil-hp on 27.04.23.
//
#include <format>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <algorithm>
#ifdef WIN32
#include <Windows.h>
auto& mcout = std::wcout;
#else
auto& mcout = std::cout;
#endif
#include "ConfigState.h"
#include "Curves/BezierCurve0.h"
#include "Curves/BezierCurve2.h"
#include "Curves/BezierCurveInter.h"
#include "ImGui/ImGuiPanel.h"
#include "ImGui/imgui_include.h"
#include "ImGuiUtil.h"
#include "Scene.h"
#include "Solids/Torus.h"
#include "Surfaces/BezierSurface0.h"
#include "Surfaces/BezierSurface2.h"
#include "Surfaces/GregoryPatch.h"
#include "Object/IntersectionObject.h"
#include "src/Json/FileLoading.h"
#include "src/Object/Point.h"

enum class SpecialPanel: short {
    None,
    FileLoadSavePanel,
    FileSaveMakeSurePanel,
    FileFailPanel
};
SpecialPanel activeSpecialPanel = SpecialPanel::None;

int getTypeValue(const std::filesystem::path& p) {
    if(p.empty())
        return 9999;
    if(is_directory(p))
        return 0;
    else if(is_regular_file(p))
        return 2;
    else
        return 1;
}

void initPath(std::filesystem::path& path, std::vector<std::filesystem::path>& files) {
#ifdef WIN32
    if(path=="") {
        files.clear();
        uint32_t drives = GetLogicalDrives();
        char letter = 'A';
        for(uint32_t i=0x1;i<=0x4000000;i<<=1) {
            std::string l = std::string(1, letter)+":/";
            if(drives&i)
                files.emplace_back(l);
            letter++;
        }
        return;
    }
#endif
    std::filesystem::directory_iterator b(path), e;
    std::vector<std::filesystem::path> paths(b, e);
    files = std::move(paths);
	//filter elements
	for(auto& f: files) {
		if(is_directory(f)) {
			try {
				std::filesystem::directory_iterator it(f);
			}
			catch(...) {
				f="";
			}
		}
		else {
			std::ifstream testFile(f);
			if(testFile.bad())
				f="";
		}
	}
    //TODO - check if file can be opened
    std::sort(files.begin(), files.end(), [](const auto& a, const auto& b) {
		int ta = getTypeValue(a);
		int tb = getTypeValue(b);
		if(ta!=tb) return getTypeValue(a)<getTypeValue(b);
		return a<b;
    });
    while(!files.empty() && files.back().empty())
        files.pop_back();
}

void changePath(std::filesystem::path& path, std::vector<std::filesystem::path>& files, const std::filesystem::path *change) {
    if(!change && path.has_parent_path()) {
#ifdef WIN32
        if(path.string().length()==3)
            path = "";
        else
#endif
        path = path.parent_path();
    }
    else if(change) {
        path = *change;
    }
    initPath(path, files);
}


ImU32 ImCol32(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
#ifdef __GNUC__
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
    setNextPanelAlignment({155, 220}, {configState.screenWidth, configState.screenHeight}, {1.f,0.f},{0,175});
    ImGui::Begin("Create object",nullptr, ImGuiWindowFlags_NoResize);
	ImGui::BeginTable("split", 1, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings);
    if(activeSpecialPanel!=SpecialPanel::None)
        ImGui::BeginDisabled();
	ImVec2 bSize = ImVec2(-FLT_MIN, 0.0f);
	ImGui::TableNextColumn();
    if(ImGui::Button("Torus", bSize)) {
        scene.objectArray.add<bf::Torus>(scene.cursor.transform);
    }
	ImGui::TableNextColumn();
    if(ImGui::Button("Point", bSize)) {
        scene.objectArray.add<bf::Point>(scene.cursor.transform);
        int redIndex = scene.objectArray.getActiveRedirector();
        if(scene.objectArray.isCorrect(redIndex)) {
            scene.objectArray[redIndex].addPoint(scene.objectArray.size()-1);
        }
    }
	ImGui::TableNextColumn();
    if(ImGui::Button("Bézier curve 0", bSize)) {
        scene.objectArray.addRef<bf::BezierCurve0>();
    }
	ImGui::TableNextColumn();
    if(ImGui::Button("Bézier curve 2", bSize)) {
        scene.objectArray.addRef<bf::BezierCurve2>();
    }
	ImGui::TableNextColumn();
    if(ImGui::Button("Bézier curve inter", bSize)) {
        scene.objectArray.addRef<bf::BezierCurveInter>();
    }
	ImGui::TableNextColumn();
	if(ImGui::Button("Bézier surface 0", bSize)) {
		scene.objectArray.addRef<bf::BezierSurface0>(scene.cursor);
	}
	ImGui::TableNextColumn();
    if(ImGui::Button("Bézier surface 2", bSize)) {
        scene.objectArray.addRef<bf::BezierSurface2>(scene.cursor);
    }
	ImGui::TableNextColumn();
	if(ImGui::Button("Gregory patch", bSize)) {
		scene.objectArray.addRef<bf::GregoryPatch>();
	}
	ImGui::TableNextColumn();
	if(ImGui::Button("Intersection", bSize)) {
		scene.objectArray.add<bf::IntersectionObject>();
	}
    if(activeSpecialPanel!=SpecialPanel::None)
        ImGui::EndDisabled();
	ImGui::EndTable();
    ImGui::End();
}

void bf::imgui::listOfObjectsPanel(bf::Scene &scene, bf::ConfigState& configState) {
#ifdef WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
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
    if(scene.objectArray.isCorrect(scene.objectArray.getActiveIndex()) && !scene.objectArray.isMultipleActive())
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

auto path = std::filesystem::current_path()/"saves";
std::vector<std::filesystem::path> files;
mstring name;
bool wasPathChosen = false;

void fileLoadSavePanel(bool isLoading, bf::Scene& scene) {
    ImVec2 size = {700, 410};
    ImGui::SetNextWindowSizeConstraints(size,size);
    ImGui::Begin("Choose file");
    ImGui::Text("%s", toStr(pathToMStr(path)).c_str());
    ImGui::BeginChild("Child", ImVec2(ImGui::GetContentRegionAvail().x, 300), true);
    if(!path.empty() && ImGui::Selectable(".."))
        changePath(path,files,nullptr);
    for (const auto & entry : files) {
        if(is_directory(entry)) {
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255,127,0,255));
            auto p = pathToMStr(entry.filename());
            if(p.empty()) {
                p = pathToMStr(entry);
            }
            if(ImGui::Selectable(toStr(p).c_str())) {
                changePath(path,files,&entry);
                ImGui::PopStyleColor();
                break;
            }
            ImGui::PopStyleColor();
        }
        else if(entry.extension()==".json") {
             if(ImGui::Selectable(toStr(pathToMStr(entry.filename())).c_str())) {
                auto tmp = entry.filename();
                if(!isLoading)
                    tmp.replace_extension("");
                name = pathToMStr(tmp);
                wasPathChosen=true;
                break;
            }
        }
    }
    ImGui::EndChild();
    if(isLoading) {
        mstring a = pathToMStr(path)+"/"_m+name;
        ImGui::Text("Chosen path: %s", toStr(a).c_str());
    }
    else {
        if(bf::imgui::checkChanged("File path (+.json)", name))
            wasPathChosen = !name.empty();
    }
    if(!wasPathChosen)
        ImGui::BeginDisabled();
    if(ImGui::Button(isLoading ? "Load" : "Save")) {
        auto fileName = pathToMStr(path)+"/"_m+name;
        if(isLoading) {
            mcout << "Chosen file to load: "_m << fileName << "\n"_m;
            std::ifstream in(fileName);
            if(in.bad() || !bf::loadFromStream(scene.objectArray, scene.camera, in))
                activeSpecialPanel = SpecialPanel::FileFailPanel;
            else
                activeSpecialPanel = SpecialPanel::None;
        }
        else {
            fileName += ".json"_m;
            if(std::filesystem::exists(fileName))
                activeSpecialPanel = SpecialPanel::FileSaveMakeSurePanel;
            else  {
                mcout << "Chosen file to save: "_m << fileName << "\n"_m;
                std::ofstream out(fileName);
                if (out.bad() || !bf::saveToStream(scene.objectArray, scene.camera, out))
                    activeSpecialPanel = SpecialPanel::FileFailPanel;
                else
                    activeSpecialPanel = SpecialPanel::None;
            }
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
            initPath(path,files);
            name = {};
            activeSpecialPanel = SpecialPanel::FileLoadSavePanel;
        }
        ImGui::SameLine();
        if (ImGui::Button("Load from file")) {
            isLoading = true;
            initPath(path,files);
            wasPathChosen = false;
            activeSpecialPanel = SpecialPanel::FileLoadSavePanel;
        }
    }
    ImGui::End();
    if(activeSpecialPanel==SpecialPanel::FileLoadSavePanel) {
        fileLoadSavePanel(isLoading, scene);
    }
    else if(activeSpecialPanel==SpecialPanel::FileSaveMakeSurePanel) {
        setNextPanelAlignment({450, 75}, {configState.screenWidth, configState.screenHeight}, {.5f,.5f});
        ImGui::Begin("File2", nullptr, ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoCollapse);
        std::string makeSureText = std::format("Are you sure you want to override file {}?", toStr(name + ".json"_m));
        ImGui::Text("%s", makeSureText.c_str());
        if(ImGui::Button("Yes")) {
            auto fileName = pathToMStr(path)+"/"_m+name+".json"_m;
            mcout << "Chosen file to save: "_m << fileName << "\n"_m;
            std::ofstream out(fileName);
            if (out.good() && bf::saveToStream(scene.objectArray, scene.camera, out))
                activeSpecialPanel = SpecialPanel::None;
            else
                activeSpecialPanel = SpecialPanel::FileFailPanel;
        }
        ImGui::SameLine();
        if(ImGui::Button("No"))
            activeSpecialPanel=SpecialPanel::None;
        ImGui::End();
    }
    if(activeSpecialPanel == SpecialPanel::FileFailPanel && !ImGui::IsPopupOpen("File fail")) {
        ImGui::OpenPopup("File fail");
    }
    if(ImGui::IsPopupOpen("File fail")) {
        setNextPanelAlignment({450, 75}, {configState.screenWidth, configState.screenHeight}, {.5f, .5f});
    }
    if(ImGui::BeginPopupModal("File fail", nullptr, ImGuiWindowFlags_NoResize)) {
        std::string failText = std::format("Failed to {} file {}!", isLoading ? "load" : "save", toStr(name));
		ImGui::TextColored({255,0,0,255},"%s", failText.c_str());
        if(ImGui::Button("OK")) {
            ImGui::CloseCurrentPopup();
            activeSpecialPanel=SpecialPanel::None;
        }
        ImGui::EndPopup();
    }
}

