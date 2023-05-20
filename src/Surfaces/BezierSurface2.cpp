//
// Created by kamil-hp on 06.05.23.
//

#include <iostream>
#include "ImGui/imgui_include.h"
#include "BezierSurface2.h"
#include "Shader/ShaderArray.h"
#include "ImGui/ImGuiUtil.h"
#include "Object/ObjectArray.h"
#include "Object/Point.h"
#include "src/Gizmos/Cursor.h"
#include "Util.h"

int bf::BezierSurface2::_index = 1;

bf::BezierSurface2::BezierSurface2(bf::ObjectArray &oArray, const std::string &objName, const bf::Cursor &c)
        : BezierSurfaceCommon(oArray, objName, c) {}

bf::BezierSurface2::BezierSurface2(bf::ObjectArray &oArray, const bf::Cursor &c) : BezierSurfaceCommon(oArray,
                                                                                                            c) {}

void bf::BezierSurface2::generatePoints(const glm::vec2 &totalSize) {
    isC2 = true;
    auto P = static_cast<int>(objectArray.size());
    //generate points
    if(!isWrappedX) {
        float dx = totalSize.x / (static_cast<float>(segs.x) + 3.f);
        float dy = totalSize.y / (static_cast<float>(segs.y) + 3.f);
        for (int i = 0; i < segs.y+3; i++) {
            for (int j = 0; j < segs.x+3; j++) {
                bf::Transform t = cursor.transform;
                glm::vec3 v(.0f);
                v.x = dx * (static_cast<float>(j));
                v.y = dy * (static_cast<float>(i));
                t.position += bf::rotate(v, transform.rotation);
                objectArray.add<bf::Point>(t);
                objectArray[objectArray.size() - 1].indestructibilityIndex = 1u;
            }
        }
        for (int i = 0; i < segs.y; i++) {
            int S = segs.x + 3;
            std::vector<pArray> segsRow;
            for (int j = 0; j < segs.x; j++) {
                segsRow.emplace_back();
                for (int k = 0; k < 4; k++) {
                    for (int l = 0; l < 4; l++) {
                        segsRow.back()[4 * k + l] = P + j + l + S * (i + k);
                    }
                }
            }
            pointIndices.emplace_back(std::move(segsRow));
        }
    }
    else {
        float dt = 2.f * PI / (static_cast<float>(segs.x));
        float dy = totalSize.y / (static_cast<float>(segs.y) + 3.f);
        for (int i = 0; i <= segs.y; i++) {
            for (int j = 0; j < segs.x+3; j++) {
                bf::Transform t = cursor.transform;
                glm::vec3 v(.0f);
                v.x = std::cos(dt * (static_cast<float>(j)));
                v.z = std::sin(dt * (static_cast<float>(j)));
                v.y = dy * (static_cast<float>(i));
                t.position += bf::rotate(v, transform.rotation);
                objectArray.add<bf::Point>(t);
                objectArray[objectArray.size() - 1].indestructibilityIndex = 1u;
            }
        }
        for (int i = 0; i < segs.y; i++) {
            int S = segs.x;
            std::vector<pArray> segsRow;
            for (int j = 0; j < segs.x; j++) {
                std::cout << j << " " << S*i << "\n";
                segsRow.emplace_back();
                for (int k = 0; k < 4; k++) {
                    for (int l = 0; l < 4; l++) {
                        segsRow.back()[4 * k + l] = P + (j + l)%S + S * (i + k);
                        std::cout << (j + l)%S + S * (i + k) << "\n";
                    }
                }
            }
            pointIndices.emplace_back(std::move(segsRow));
        }
    }
}
