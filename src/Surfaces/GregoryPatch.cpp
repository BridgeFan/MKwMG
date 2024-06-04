//
// Created by kamil-hp on 28.05.23.
//

#include "GregoryPatch.h"
#include "ImGui/ImGuiUtil.h"
#include "ImGui/imgui_include.h"
#include "Object/ObjectArray.h"
#include "Shader/ShaderArray.h"
#include "Surfaces/BezierSurface0.h"
#include "Util.h"
#include <algorithm>
#include <iostream>
#include <optional>
#include <set>

bool isCorrectIndex(const bf::BezierSurfaceSegment& s, uint8_t index) {
	return (s.isLeftEmpty()	&&	index%2==0) ||
		   (s.isRightEmpty() &&	index%2==1) ||
		   (s.isBottomEmpty() && index<=1) ||
		   (s.isTopEmpty() && index>=2);
}

using SegmentSurface = std::pair<bf::BezierSurface0*, bf::BezierSurfaceSegment*>;

std::optional<std::array<uint8_t, 6> > areGregoryIntersecting(bf::BezierSurfaceSegment* a, bf::BezierSurfaceSegment* b, bf::BezierSurfaceSegment* c) {
	if(!a || !b || !c) return std::nullopt;
	std::array<uint8_t, 6> order{};
	constexpr std::array A=bf::GregoryPatch::tmpArray;
	//TODO: checking diagonal
	for(uint8_t i1: A) {
		for(uint8_t i2: A) {
			if(i1==i2) continue;
			for(uint8_t i3: A) {
				for(uint8_t i4: A) {
					if(i3==i4) continue;
					for(uint8_t i5: A) {
						for(uint8_t i6: A) {
							if(i5==i6) continue;
							if(a->pointIndices[i2]==b->pointIndices[i3] && b->pointIndices[i4]==c->pointIndices[i5] && c->pointIndices[i6]==a->pointIndices[i1]) {
								order={i1,i2,i3,i4,i5,i6};
								return order;
							}
						}
					}
				}
			}
		}
	}
	return std::nullopt;
}

struct FoundGregoryStruct {
	std::array<bf::BezierSurface0*, 3> surfaces;
	std::array<bf::BezierSurfaceSegment*, 3> segments;
	std::array<uint8_t, 6> order;
	bool operator==(const FoundGregoryStruct& other) {
		//TODO - more than 1 Grégory patch from particular three segments (checking order)
		for (int i = 0; i < 3; i++)/*surfaces*/ {
			for(int j=0;j<3;j++) {
				auto m=(i+j)%3;
				if(surfaces[j]!=other.surfaces[m] || segments[j]!=other.segments[m]) //different segments are used
					continue;
				else
					return true;
			}
		}
		return false;
	}
};
FoundGregoryStruct toFoundStruct(const bf::GregoryPatch& patch) {
	FoundGregoryStruct str{};
	str.surfaces = patch.getSurfaces();
	str.segments = patch.getSegments();
	str.order = patch.getOrder();
	return str;
}

bool operator==(const bf::GregoryPatch& patch, const FoundGregoryStruct& other) {
	return toFoundStruct(patch)==other;
}
bool operator==(const FoundGregoryStruct& other, const bf::GregoryPatch& patch) {
	return patch==other;
}


std::vector<FoundGregoryStruct> findGregories(const std::vector<bf::BezierSurface0*> surfaces) {
	if(surfaces.empty()) return {};
	std::vector<SegmentSurface> segs;
	std::vector<FoundGregoryStruct> foundGregories;
	for(auto& s: surfaces) {
		if(!s) continue;
		for (auto &segRow: s->segments) {
			for(auto& seg: segRow) {
				if(!seg.isInternal())
					segs.emplace_back(s, &seg);
			}
		}
	}
	//finding Gregories
	for(auto& s1: segs) {
		for(auto& s2: segs) {
			if(s1==s2) continue;
			for(auto& s3: segs) {
				if(s1==s3 || s2==s3) continue;
				auto res = areGregoryIntersecting(s1.second,s2.second,s3.second);
				if(res) {
					FoundGregoryStruct str;
					str.surfaces = {s1.first, s2.first, s3.first};
					str.segments = {s1.second, s2.second, s3.second};
					str.order = *res;
					foundGregories.emplace_back(std::move(str));
				}
			}
		}
	}
	return foundGregories;
}

std::vector<FoundGregoryStruct> gregoryCandidates;

namespace bf {
	void GregoryPatch::onMergePoints(int p1, int p2) {}
	GregoryPatch::GregoryPatch(ObjectArray &oArray) : ObjectArrayListener(oArray) {
		std::vector<bf::BezierSurface0*> sfs;
		std::vector<bf::GregoryPatch*> gregories;
		for(unsigned i=0;i<objectArray.size();i++) {
			if(!objectArray.isCorrect(i)) continue;
			auto& o = objectArray[i];
			if(objectArray.isActive(i) && typeid(o)==typeid(bf::BezierSurface0)) {
				sfs.emplace_back(dynamic_cast<bf::BezierSurface0*>(&o));
			}
			else if(typeid(o)==typeid(bf::GregoryPatch) && &o!=this) {
				gregories.emplace_back(dynamic_cast<bf::GregoryPatch*>(&o));
			}
		}
		gregoryCandidates = findGregories(sfs);
		bool pr = true;
		while(!gregoryCandidates.empty() && pr) {
			pr = false;
			for(auto g: gregories) {
				if(*g==gregoryCandidates.back()) {
					gregoryCandidates.pop_back();
					pr=true;
					break;
				}
			}
		}
		if(gregoryCandidates.empty())
			return;
		else {
			//choosing first feasible candidate
			auto& chosenGregory = gregoryCandidates[0];
			order=std::move(chosenGregory.order);
			segments=std::move(chosenGregory.segments);
			surfaces=std::move(chosenGregory.surfaces);
			for(int i=0;i<3;i++) {
				if(surfaces[i])
					surfaces[i]->indestructibilityIndex++;
			}
			gregoryCandidates.clear();
		}
	}
	void GregoryPatch::onRemoveObject(unsigned int index) {}
	void GregoryPatch::onMoveObject(unsigned int index) {
		if(!objectArray.isCorrect(index))
			return;
		bool wasChange = false;
		for(auto& s: segments) {
			for(auto& p: s->pointIndices) {
				if(p==index) {
					wasChange = true;
					break;
				}
			}
			if(wasChange)
				break;
		}
		if(wasChange)
			recalculate();
	}
	bool GregoryPatch::postInit() {
		if(segments[0]==nullptr) return true;
		recalculate();
		return false;
	}
	void GregoryPatch::draw(const ShaderArray &shaderArray) const {
		if(!segments[0] || shaderArray.getActiveIndex()!=BasicShader) {
			return;
		}
		if(shaderArray.getActiveIndex()==BasicShader && isDebug) {
			shaderArray.setColor(255u,0u,0u);
			Solid::draw(shaderArray);
		}
		/*segments[0]->draw(shaderArray);
		segments[1]->draw(shaderArray);
		segments[2]->draw(shaderArray);*/
	}
	GregoryPatch::~GregoryPatch() {
		for(auto s: surfaces) {
			if(s)
				s->indestructibilityIndex--;
		}
	}
	void GregoryPatch::ObjectGui() {
		bf::imgui::checkChanged("Gregory patch name", name);
		std::string tmp;
		/*for(int8_t i=14;i>=0;i-=2) {
			tmp+=(static_cast<char>('0'+(order>>i&3)));
		}*/
		ImGui::Text("Order: %s", tmp.c_str());
		ImGui::Checkbox("Debug", &isDebug);
	}
	void GregoryPatch::recalculate() {
		vertices.clear();
		indices.clear();
		for(int i=0;i<9;i++) {
			indices.push_back(i);
			indices.push_back((i+1)%9);
		}
		//indices.resize(3, 0u);
		for(int i=0;i<3;i++) {
			if(!segments[i]) {vertices.clear(); indices.clear(); break;}
			auto& s = *segments[i];
			vertices.emplace_back(objectArray[s.pointIndices[order[2*i]]].getPosition());
			vertices.emplace_back(objectArray[s.pointIndices[lerp(order[2*i],order[2*i+1],0.334f)]].getPosition());
			vertices.emplace_back(objectArray[s.pointIndices[lerp(order[2*i],order[2*i+1],0.667f)]].getPosition());
		}
		setBuffers();
		//TODO
	}
	const std::array<bf::BezierSurfaceSegment *, 3> &GregoryPatch::getSegments() const {
		return segments;
	}
	bool GregoryPatch::isMovable() const {
		return false;
	}
	const std::array<bf::BezierSurface0 *, 3> &GregoryPatch::getSurfaces() const {
		return surfaces;
	}
}// namespace bf
