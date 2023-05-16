//
// Created by kamil-hp on 06.05.23.
//

#include "ImGui/imgui_include.h"
#include "BezierSurface0.h"
#include "Shader/ShaderArray.h"
#include "ImGui/ImGuiUtil.h"
#include "Object/ObjectArray.h"
#include "src/Gizmos/Cursor.h"

int bf::BezierSurface0::_index = 1;

void bf::BezierSurface0::recalculateSegments(unsigned int index) {
    for(auto& sRow: segments) {
        for(auto& s: sRow) {
            s.onPointMove(objectArray, index);
        }
    }
}

void bf::BezierSurface0::initSegments(std::vector<std::vector<std::string> >&& segmentNames,
        std::vector<std::vector<glm::vec<2,int> > >&& segmentSamples) {
    for(unsigned i=0;i<pointIndices.size();i++) {
        std::vector<bf::BezierSurfaceSegment0> segRow;
        for(unsigned j=0;j<pointIndices[i].size();j++) {
            segRow.emplace_back();
            if(!segmentNames.empty())
                segRow.back().name = std::move(segmentNames[i][j]);
            if(!segmentSamples.empty())
                segRow.back().samples = std::move(segmentSamples[i][j]);
            else
                segRow.back().samples = samples;
            segRow.back().pointIndices = pointIndices[i][j];
            segRow.back().initGL(objectArray);
        }
        segments.emplace_back(std::move(segRow));
    }
    segmentNames.clear();
    segmentSamples.clear();
}

bf::BezierSurface0::BezierSurface0(bf::ObjectArray &oArray, const std::string &objName, const bf::Cursor &c)
        : BezierSurfaceCommon(oArray, objName, c) {}

bf::BezierSurface0::BezierSurface0(bf::ObjectArray &oArray, const bf::Cursor &c) : BezierSurfaceCommon(oArray,
                                                                                                            c) {}
