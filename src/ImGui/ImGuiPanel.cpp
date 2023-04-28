//
// Created by kamil-hp on 27.04.23.
//
#include "ImGuiUtil.h"
#include "imgui-master/imgui.h"
#include "ImGui/ImGuiPanel.h"
#include "Scene.h"
#include "ConfigState.h"
#include "Solids/Torus.h"
#include "Solids/Point.h"
#include "Curves/BezierCurve0.h"
#include "Curves/BezierCurve2.h"
#include "Curves/BezierCurveInter.h"
#include "FileLoading.h"

void bf::imgui::createObjectPanel(Scene &scene) {
    ImGui::Begin("Create object");
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
    ImGui::End();
}

void bf::imgui::listOfObjectsPanel(bf::Scene &scene, bf::ConfigState& configState) {
    ImGui::Begin("List of objects");
    ImGui::Text("Hold CTRL and click to select multiple items.");
    ImGui::Checkbox("Uniform scalng", &configState.isUniformScaling);
    for(int i=0;i<3;i++) {
        static bool tmp[3];
        constexpr char axisName[] = {'X', 'Y', 'Z'};
        tmp[i]=configState.isAxesLocked&(0x1<<i);
        if(ImGui::Checkbox((std::string("Lock ")+axisName[i]).c_str(), &tmp[i])) {
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
        if (scene.objectArray.imGuiCheckChanged(n, scene.multiCursor))
        {
            scene.multiCursor.transform = bf::Transform::Default;
            if (!configState.isCtrlPressed) { // Clear selection when CTRL is not held
                scene.objectArray.clearSelection(n);
            }
        }
    }
    ImGui::End();
}

void bf::imgui::modifyObjectPanel(bf::Scene &scene, const bf::ConfigState& configState) {
    ImGui::Begin("Modify object panel");
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
    ImGui::End();
}

void bf::imgui::cameraInfoPanel(bf::Scene &scene, bf::ConfigState& configState) {
    ImGui::Begin("Camera info");
    scene.camera.ObjectGui(configState);
    if(ImGui::Button("Save to file"))
        bf::saveToFile("../save.json",scene.objectArray);
    if(ImGui::Button("Load from file"))
        bf::loadFromFile("../save.json",scene.objectArray);
    ImGui::End();
}

