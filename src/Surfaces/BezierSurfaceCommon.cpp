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
#include "Util.h"

int bf::BezierSurfaceCommon::_index = 1;

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
        if(isCylinder) {
            bf::imgui::checkChanged("Radius", totalSize.x);
            bf::imgui::checkChanged("Height", totalSize.y);
        }
        else {
            bf::imgui::checkChanged("Size X", totalSize.x);
            bf::imgui::checkChanged("Size Y", totalSize.y);
        }
        if(ImGui::Button("Confirm")) {
            objectArray.isForcedActive=false;
            auto P = static_cast<int>(objectArray.size());
            //generate points
            if(!isCylinder) {
                float dx = totalSize.x / static_cast<float>(segs.x) / 3.f;
                float dy = totalSize.y / static_cast<float>(segs.y) / 3.f;
                for (int i = 0; i <= 3 * segs.y; i++) {
                    for (int j = 0; j <= 3 * segs.x; j++) {
                        bf::Transform t = cursor.transform;
                        glm::vec3 v(.0f);
                        v.x = dx * (static_cast<float>(j));
                        v.y = dy * (static_cast<float>(i));
                        t.position += v;
                        objectArray.add<bf::Point>(t);
                        objectArray[objectArray.size() - 1].indestructibilityIndex = 1u;
                    }
                }
                for (int i = 0; i < segs.y; i++) {
                    int S = segs.x * 3 + 1;
                    std::vector<pArray> segsRow;
                    for (int j = 0; j < segs.x; j++) {
                        segsRow.emplace_back();
                        for (int k = 0; k < 4; k++) {
                            for (int l = 0; l < 4; l++) {
                                segsRow.back()[4 * k + l] = P + 3 * j + l + S * (3 * i + k);
                            }
                        }
                    }
                    pointIndices.emplace_back(std::move(segsRow));
                }
            }
            else {
                isWrappedX = true;
                float dt = 2.f * PI / (3.f * static_cast<float>(segs.x));
                float dy = totalSize.y / static_cast<float>(segs.y) / 3.f;
                for (int i = 0; i <= 3 * segs.y; i++) {
                    for (int j = 0; j < 3 * segs.x; j++) {
                        bf::Transform t = cursor.transform;
                        glm::vec3 v(.0f);
                        v.x = std::cos(dt * (static_cast<float>(j)));
                        v.z = std::sin(dt * (static_cast<float>(j)));
                        v.y = dy * (static_cast<float>(i));
                        t.position += v;
                        objectArray.add<bf::Point>(t);
                        objectArray[objectArray.size() - 1].indestructibilityIndex = 1u;
                    }
                }
                for (int i = 0; i < segs.x; i++) {
                    int S = segs.x * 3;
                    std::vector<pArray> segsRow;
                    for (int j = 0; j < segs.y; j++) {
                        segsRow.emplace_back();
                        for (int k = 0; k < 4; k++) {
                            for (int l = 0; l < 4; l++) {
                                segsRow.back()[4 * k + l] = P + (3 * i + l)%S + S * (3 * j + k);
                            }
                        }
                    }
                    pointIndices.emplace_back(std::move(segsRow));
                }
            }
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
                if(objectArray[i].indestructibilityIndex>0)
                    objectArray[i].indestructibilityIndex--;
            }
        }
    }
}