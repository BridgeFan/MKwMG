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
	updateDebug();
}

void bf::BezierSurfaceCommon::draw(const bf::ShaderArray& shaderArray) const {
    int index=0;
	drawDebug(shaderArray);
    if(activeIndex>=0) {
        auto s = static_cast<int>(segments[0].size());
        segments[activeIndex/s][activeIndex%s].segmentDraw(shaderArray, isPolygonVisible, isSurfaceVisible, true);
    }
    for(const auto& sRow: segments) {
        for(const auto& s: sRow) {
            if(index!=activeIndex)
                s.segmentDraw(shaderArray, isPolygonVisible, isSurfaceVisible, false);
            index++;
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
            auto pts = generatePoints(totalSize);
            objectArray.isForcedActive=false;
            surfacePostInit(std::move(pts));
        }
    }
    else {
        ImGui::Text("Visible: ");
        ImGui::SameLine();
        ImGui::Checkbox("Polygon", &isPolygonVisible);
        ImGui::SameLine();
        ImGui::Checkbox("Surface", &isSurfaceVisible);
        if(activeIndex>=0) {
            auto s = static_cast<int>(segments[0].size());
            auto& seg = segments[activeIndex/s][activeIndex%s];
            ImGui::InputText("Seg. name", &seg.name);
            bf::imgui::checkChanged("Seg. samples", seg.samples);
            if(ImGui::Button("Deselect")) {
                activeIndex=-1;
            }
        }
        else {
            if (bf::imgui::checkChanged("Global samples", samples)) {
                for (auto &sRow: segments) {
                    for (auto &s: sRow) {
                        s.samples = samples;
                    }
                }
            }
        }
        ImGui::BeginChild("List of segments", ImVec2(ImGui::GetContentRegionAvail().x, activeIndex>=0 ? 150.f : 195.f), true);
        for(unsigned i=0u; i < segments.size(); i++) {
            for(unsigned j=0u;j<segments[i].size();j++) {
                int index = static_cast<int>(i*segments[i].size()+j);
                bool val = (index == activeIndex);
                bool ret = bf::imgui::checkSelectableChanged(segments[i][j].name.c_str(), index, val);
                if (ret && val) {
                    activeIndex = index;
                }
            }
        }
        ImGui::EndChild();
    }
}

bool bf::BezierSurfaceCommon::isMovable() const {
    return false;
}

bf::ShaderType bf::BezierSurfaceCommon::getShaderType() const {
    return bf::ShaderType::MultipleShaders;
}

bf::BezierSurfaceCommon::BezierSurfaceCommon(bf::ObjectArray &objArray, const std::string &objName, const bf::Cursor& c): Solid(objName),
                                                                                                                bf::ObjectArrayListener(objArray), cursor(c), samples(4,4) {
    objectArray.isForcedActive=true;
}

bf::BezierSurfaceCommon::BezierSurfaceCommon(bf::ObjectArray &objArray, const bf::Cursor& c):
        BezierSurfaceCommon(objArray, "Bézier surface0 " + std::to_string(_index), c)
{_index++;}

bool bf::BezierSurfaceCommon::postInit() {
    surfacePostInit({});
	return false;
}
void bf::BezierSurfaceCommon::surfacePostInit(std::vector<std::vector<pArray> >&& pointIndices) {
    if(segments.empty())
        initSegments({},{}, std::move(pointIndices));
    for(auto& sRow: segments) {
        for(auto& s: sRow) {
            s.initGL(objectArray);
        }
    }
	if(isC2 || segments.empty()) return;
	if(!isWrappedY) {
		for (auto &s: segments[0]) {
			s.emptyEdges |= 0x1;
		}
		for (auto &s: segments.back()) {
			s.emptyEdges |= 0x2;
		}
	}
	if(!isWrappedX) {
		for (auto &sRow: segments) {
			if (sRow.empty())
				continue;
			sRow[0].emptyEdges |= 0x4;
			sRow.back().emptyEdges |= 0x8;
		}
	}
}

void bf::BezierSurfaceCommon::onRemoveObject(unsigned int index) {
    for(auto& sRow: segments) {
        for(auto& s: sRow) {
            for(auto& i: s.pointIndices) {
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
    for(auto& sRow: segments) {
        for(auto& s: sRow) {
            for(auto& i: s.pointIndices) {
                if(objectArray.isCorrect(i) && objectArray[i].indestructibilityIndex>0)
                    objectArray[i].indestructibilityIndex--;
            }
        }
    }
}

void bf::BezierSurfaceCommon::initSegments(std::vector<std::vector<std::string> >&& segmentNames,
                                      std::vector<std::vector<glm::vec<2,int> > >&& segmentSamples,
        std::vector<std::vector<pArray> >&& pointIndices) {
    for(unsigned i=0;i<pointIndices.size();i++) {
        std::vector<bf::BezierSurfaceSegment> segRow;
        for(unsigned j=0;j<pointIndices[i].size();j++) {
            segRow.emplace_back(isC2);
			auto& s=segRow.back();
			s.tmpIndices.reserve(4);
			bool isLeftFree=false, isRightFree=false, isBottomFree=false, isTopFree=false;
			if(j==0 && !isWrappedX) isLeftFree=true;
			if(j==pointIndices[i].size()-1 && !isWrappedX) isRightFree=true;
			if(i==0 && !isWrappedY) isBottomFree=true;
			if(i==pointIndices.size()-1 && !isWrappedY) isTopFree=true;
			if(isLeftFree || isBottomFree)
				s.tmpIndices.push_back(0);
			if(isRightFree || isBottomFree)
				s.tmpIndices.push_back(3);
			if(isLeftFree || isTopFree)
				s.tmpIndices.push_back(12);
			if(isRightFree || isTopFree)
				s.tmpIndices.push_back(15);
            if(!segmentNames.empty())
                s.name = std::move(segmentNames[i][j]);
            if(!segmentSamples.empty())
                s.samples = std::move(segmentSamples[i][j]);
            else
                s.samples = samples;
            s.pointIndices = pointIndices[i][j];
            s.initGL(objectArray);
        }
        segments.emplace_back(std::move(segRow));
    }
    segmentNames.clear();
    segmentSamples.clear();
	updateDebug();
}
void bf::BezierSurfaceCommon::onMergePoints(int p1, int p2) {
	if(objectArray.isCorrect(p2) && objectArray[p2].indestructibilityIndex>0)
		objectArray[p2].indestructibilityIndex--;
	for(auto& sRow: segments) {
		for(auto& s: sRow) {
			for(auto& i: s.pointIndices) {
				if(static_cast<int>(i)==p2)
					i=p1;
			}
		}
	}
	onMoveObject(p1);
}
std::tuple<int, int, double, double> bf::BezierSurfaceCommon::setParameters(double uf, double vf) const {
	bf::vec4d param = clampParam(uf,vf,1.f);
	int iu=int(param.z);
	int iv=int(param.w);
	if(iu>=segments[0].size()) {
		iu--;
		param.x+=1.f;
	}
	if(iv>=segments.size()) {
		iv--;
		param.y+=1.f;
	}
	return {iu,iv,param.x, param.y};
}
