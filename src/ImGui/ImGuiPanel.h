#pragma once
//
// Created by kamil-hp on 27.04.23.
//

#ifndef MG1_ZAD2_IMGUIPANEL_H
#define MG1_ZAD2_IMGUIPANEL_H
namespace bf {
    class Scene;
    class ConfigState;
    namespace imgui {
        void createObjectPanel(Scene& scene, const bf::ConfigState& configState);
        void listOfObjectsPanel(Scene& scene, bf::ConfigState& configState);
        void modifyObjectPanel(Scene& scene, const bf::ConfigState& configState);
        void cameraInfoPanel(Scene& scene, bf::ConfigState& configState);
    }
}
#endif //MG1_ZAD2_IMGUIPANEL_H
