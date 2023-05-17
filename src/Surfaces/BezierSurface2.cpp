//
// Created by kamil-hp on 06.05.23.
//

#include "ImGui/imgui_include.h"
#include "BezierSurface2.h"
#include "Shader/ShaderArray.h"
#include "ImGui/ImGuiUtil.h"
#include "Object/ObjectArray.h"
#include "src/Gizmos/Cursor.h"
#include <iostream>

int bf::BezierSurface2::_index = 1;

void bf::BezierSurface2::recalculateSegments(unsigned int index) {
    //TODO: Bézier surface C2 recalculate
    std::cout << index << "\n";
}

void bf::BezierSurface2::initSegments(std::vector<std::vector<std::string> >&& segmentNames,
        std::vector<std::vector<glm::vec<2,int> > >&& segmentSamples) {
    //TODO: Bézier surface C2 init segments
    std::cout << segmentNames.size() << " " << segmentSamples.size() << "\n";
}

bf::BezierSurface2::BezierSurface2(bf::ObjectArray &oArray, const std::string &objName, const bf::Cursor &c)
        : BezierSurfaceCommon(oArray, objName, c) {}

bf::BezierSurface2::BezierSurface2(bf::ObjectArray &oArray, const bf::Cursor &c) : BezierSurfaceCommon(oArray,
                                                                                                            c) {}
