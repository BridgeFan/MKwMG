//
// Created by kamil-hp on 16.05.23.
//

#include "BezierSurfaceCommon.h"
#include "ImGui/imgui_include.h"
#include "Shader/ShaderArray.h"
#include "ImGui/ImGuiUtil.h"
#include "Object/ObjectArray.h"
#include "src/Gizmos/Cursor.h"
#include "Object/Point.h"

int bf::BezierSurfaceCommon::_index = 1;

void bf::BezierSurfaceCommon::recalculateSegments(unsigned int index) {
    for(auto& sRow: segments) {
        for(auto& s: sRow) {
            s.onPointMove(objectArray, index);
        }
    }
}

void bf::BezierSurfaceCommon::draw(const bf::ShaderArray& shaderArray) const {
    for(const auto& sRow: segments) {
        for(const auto& s: sRow) {
            s.segmentDraw(shaderArray, isPolygonVisible, isSurfaceVisible);
        }
    }
}

void bf::BezierSurfaceCommon::ObjectGui() {
    bf::imgui::checkChanged("Bezier surface name", name);
    static glm::vec2 totalSize={5.f,5.f};
    static bool isCylinder=false;
    if(segments.empty()) {
        bf::imgui::checkChanged("Samples", samples);
        ImGui::Checkbox("Cylindrical", &isCylinder);
        bf::imgui::checkChanged("Segments", segs);
        bf::imgui::checkChanged("Rotation", transform.rotation);
        if(isCylinder) {
            bf::imgui::checkChanged("Radius", totalSize.x);
            bf::imgui::checkChanged("Height", totalSize.y);
        }
        else {
            bf::imgui::checkChanged("Size X", totalSize.x);
            bf::imgui::checkChanged("Size Y", totalSize.y);
        }
        if(ImGui::Button("Confirm")) {
            if(isCylinder)
                isWrappedX=true;
            objectArray.isForcedActive=false;
            generatePoints(totalSize);
            objectArray.isForcedActive=false;
            postInit();
        }
    }
    else {
        ImGui::Text("Visible: ");
        ImGui::SameLine();
        ImGui::Checkbox("Polygon", &isPolygonVisible);
        ImGui::SameLine();
        ImGui::Checkbox("Surface", &isSurfaceVisible);
        if(bf::imgui::checkChanged("Samples", samples)) {
            for(auto& sRow: segments) {
                for(auto& s: sRow) {
                    s.samples=samples;
                }
            }
        }
    }
}

bool bf::BezierSurfaceCommon::isMovable() const {
    return false;
}

bf::ShaderType bf::BezierSurfaceCommon::getShaderType() const {
    return bf::ShaderType::MultipleShaders;
}

bf::BezierSurfaceCommon::BezierSurfaceCommon(bf::ObjectArray &objArray, const std::string &objName, const bf::Cursor& c): Object(objName),
                                                                                                                bf::ObjectArrayListener(objArray), cursor(c), samples(4,4) {
    objectArray.isForcedActive=true;
}

bf::BezierSurfaceCommon::BezierSurfaceCommon(bf::ObjectArray &objArray, const bf::Cursor& c):
        BezierSurfaceCommon(objArray, "Bézier surface0 " + std::to_string(_index), c)
{_index++;}

void bf::BezierSurfaceCommon::postInit() {
    initSegments({},{});
    for(auto& sRow: segments) {
        for(auto& s: sRow) {
            s.initGL(objectArray);
        }
    }
}

void bf::BezierSurfaceCommon::onRemoveObject(unsigned int index) {
    for(auto& pRow: pointIndices) {
        for(auto& p: pRow) {
            for(auto& i: p) {
                if(i>index)
                    i--;
            }
        }
    }
    recalculateSegments(UINT_MAX);
}

void bf::BezierSurfaceCommon::onMoveObject(unsigned int index) {
    recalculateSegments(index);
}

bf::BezierSurfaceCommon::~BezierSurfaceCommon() {
    //remove point indestructibility
    for(auto& pRow: pointIndices) {
        for(auto& p: pRow) {
            for(auto& i: p) {
                if(objectArray.isCorrect(i) && objectArray[i].indestructibilityIndex>0)
                    objectArray[i].indestructibilityIndex--;
            }
        }
    }
}

void bf::BezierSurfaceCommon::initSegments(std::vector<std::vector<std::string> >&& segmentNames,
                                      std::vector<std::vector<glm::vec<2,int> > >&& segmentSamples) {
    for(unsigned i=0;i<pointIndices.size();i++) {
        std::vector<bf::BezierSurfaceSegment> segRow;
        for(unsigned j=0;j<pointIndices[i].size();j++) {
            segRow.emplace_back(isC2);
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
