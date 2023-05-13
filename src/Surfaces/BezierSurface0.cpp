//
// Created by kamil-hp on 06.05.23.
//

#include "ImGui/imgui_include.h"
#include "BezierSurface0.h"
#include "Shader/ShaderArray.h"
#include "ImGui/ImGuiUtil.h"
#include "Object/ObjectArray.h"
#include "src/Gizmos/Cursor.h"
#include "Object/Point.h"
#include "Util.h"

int bf::BezierSurface0::_index = 1;

void bf::BezierSurface0::draw(const bf::ShaderArray& shaderArray) const {
    for(const auto& s: segments) {
        s.segmentDraw(shaderArray, isPolygonVisible, isSurfaceVisible);
    }
}

void bf::BezierSurface0::ObjectGui() {
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
                    for (int j = 0; j < segs.x; j++) {
                        segments.emplace_back();
                        segments.back().samples = samples;
                        for (int k = 0; k < 4; k++) {
                            for (int l = 0; l < 4; l++) {
                                segments.back().pointIndices[4 * k + l] = P + 3 * j + l + S * (3 * i + k);
                            }
                        }
                    }
                }
            }
            else {
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
                    for (int j = 0; j < segs.y; j++) {
                        segments.emplace_back();
                        segments.back().samples = samples;
                        for (int k = 0; k < 4; k++) {
                            for (int l = 0; l < 4; l++) {
                                segments.back().pointIndices[4 * k + l] = P + (3 * i + l)%S + S * (3 * j + k);
                            }
                        }
                    }
                }
            }
            objectArray.isForcedActive=false;
			postInit();
        }
    }
    else {
        if(bf::imgui::checkChanged("Samples", samples)) {
            for(auto& s: segments) {
                s.samples=samples;
            }
        }
        ImGui::Checkbox("Polygon visible", &isPolygonVisible);
        ImGui::Checkbox("Curve visible", &isSurfaceVisible);
    }
}

bool bf::BezierSurface0::isMovable() const {
	return false;
}

bf::ShaderType bf::BezierSurface0::getShaderType() const {
	return bf::ShaderType::MultipleShaders;
}

bf::BezierSurface0::BezierSurface0(bf::ObjectArray &objArray, const std::string &objName, const bf::Cursor& c): Object(objName),
	bf::ObjectArrayListener(objArray), cursor(c), samples(4,4) {
    objectArray.isForcedActive=true;
}

bf::BezierSurface0::BezierSurface0(bf::ObjectArray &objArray, const bf::Cursor& c):
		BezierSurface0(objArray, "Bézier surface0 " + std::to_string(_index), c)
		{_index++;}

void bf::BezierSurface0::postInit() {
    for(auto& s: segments)
        s.initGL(objectArray);
}

void bf::BezierSurface0::onRemoveObject(unsigned int index) {
    for(auto& s: segments) {
        s.onPointRemove(index);
    }
}

void bf::BezierSurface0::onMoveObject(unsigned int index) {
    for(auto& s: segments) {
        s.onPointMove(objectArray, index);
    }
}

bf::BezierSurface0::~BezierSurface0() {
    //remove point indestructibility
    for(auto& s: segments) {
        for(auto& i: s.pointIndices) {
            if(objectArray[i].indestructibilityIndex>0)
                objectArray[i].indestructibilityIndex--;
        }
    }
}
