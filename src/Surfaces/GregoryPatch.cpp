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
#include <GL/glew.h>
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


bool areIndicesOK(uint8_t a, uint8_t b) {
	if(a==b) return false;
	const uint8_t& na=std::min(a,b);
	const uint8_t& nb=std::max(a,b);
	return !((na==0 && nb==15) || (na==3 && nb==12));
}

std::vector<std::array<uint8_t, 6> > areGregoryIntersecting(bf::BezierSurfaceSegment* a, bf::BezierSurfaceSegment* b, bf::BezierSurfaceSegment* c) {
	if(!a || !b || !c) return {};
	std::vector<std::array<uint8_t, 6> > ret;
	for(uint8_t i1: a->tmpIndices) {
		for(uint8_t i2: a->tmpIndices) {
			if(!areIndicesOK(i1,i2)) continue;
			for(uint8_t i3: b->tmpIndices) {
				for(uint8_t i4: b->tmpIndices) {
					if(!areIndicesOK(i3,i4)) continue;
					for(uint8_t i5: c->tmpIndices) {
						for(uint8_t i6: c->tmpIndices) {
							if(!areIndicesOK(i5,i6)) continue;
							if(a->pointIndices[i2]==b->pointIndices[i3] && b->pointIndices[i4]==c->pointIndices[i5] && c->pointIndices[i6]==a->pointIndices[i1]) {
								std::array<uint8_t, 6> order={i1,i2,i3,i4,i5,i6};
								ret.emplace_back(std::move(order));
							}
						}
					}
				}
			}
		}
	}
	return ret;
}

struct FoundGregoryStruct {
	std::array<bf::BezierSurface0*, 3> surfaces;
	std::array<bf::BezierSurfaceSegment*, 3> segments;
	std::array<uint8_t, 6> order;

	bool operator==(const FoundGregoryStruct& other) {
		if(this==&other) return true;
		for (int i = 0; i < 6; i++)/*checking movement*/ {
			bool wasBreak=true;
			for(int j=0;j<3;j++) {//segment of first struct
				int movedJ = i<3 ? (i+j)%3 : (i+6-j)%3;
				if(surfaces[j]!=other.surfaces[movedJ] || segments[j]!=other.segments[movedJ]) {
					wasBreak=false;
					break;
				}
			}
			if(wasBreak) {
				for(int j=0;j<6;j++) {
					int movedJ = i<3 ? (2*i+j)%6 : (7+2*i-j)%6;
					if(order[j]!=other.order[movedJ]) {
						wasBreak=false;
						break;
					}
				}
				if(wasBreak) return true;
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
	FoundGregoryStruct str(toFoundStruct(patch));
	return str==other;
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
	for(unsigned i=0;i<segs.size();i++) {
		const auto& s1=segs[i];
		for(unsigned j=i+1;j<segs.size();j++) {
			const auto& s2=segs[j];
			for(unsigned k=j+1;k<segs.size();k++) {
				const auto& s3=segs[j];
				auto res = areGregoryIntersecting(s1.second,s2.second,s3.second);
				for(auto& r: res) {
					FoundGregoryStruct str;
					str.surfaces = {s1.first, s2.first, s3.first};
					str.segments = {s1.second, s2.second, s3.second};
					str.order = std::move(r);
					foundGregories.emplace_back(std::move(str));
				}
			}
		}
	}
	return foundGregories;
}

std::vector<FoundGregoryStruct> gregoryCandidates;

namespace bf {
	void GregoryPatch::onMergePoints(int, int) {/*no points used*/}
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
			auto& chosenGregory = gregoryCandidates.back();
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
	void GregoryPatch::onRemoveObject(unsigned int) {/*no reaction - dependent objects indestructible*/}
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
		if(!segments[0] || (shaderArray.getActiveIndex()!=BasicShader && shaderArray.getActiveIndex()!=GregoryShader)) {
			return;
		}
		if(shaderArray.getActiveIndex()==BasicShader && isDebug) {
			shaderArray.setColor(255u,0u,0u);
			//function assumes set projection and view matrices
			glBindVertexArray(VAO);
			shaderArray.getActiveShader().setMat4("model", getModelMatrix(/*relativeTo*/));
			glDrawElements(GL_LINES, indices.size()-60, GL_UNSIGNED_INT,   // type
						   reinterpret_cast<void*>(60*sizeof(int))           // element array buffer offset
			);
		}
		else if(shaderArray.getActiveIndex()==GregoryShader) {
			glPatchParameteri( GL_PATCH_VERTICES, 20);
			const auto& shader = shaderArray.getActiveShader();
			shader.setInt("Segments",samples);
			shader.setVec2("DivisionBegin",{.0f,.0f});
			shader.setVec2("DivisionEnd",{1.f,1.f});
			glBindVertexArray(VAO);
			glDrawElements(GL_PATCHES, 60, GL_UNSIGNED_INT,   // type
							   0);           // element array buffer offset)
		}
	}
	GregoryPatch::~GregoryPatch() {
		for(auto s: surfaces) {
			if(s)
				s->indestructibilityIndex--;
		}
	}
	void GregoryPatch::ObjectGui() {
		bf::imgui::checkChanged("Gregory patch name", name);
		bf::imgui::checkChanged("Samples", samples, 1, 100);
		ImGui::Checkbox("Debug", &isDebug);
	}
	int xDiff, yDiff;

	glm::vec3 bezier2(float t, const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2) {
		return p0*(1-t)*(1-t)+2.f*p1*t*(1-t)+p2*t*t;
	}

	void GregoryPatch::recalculate() {
		vertices.clear();
		indices.clear();
		indices.resize(156);
		int _i=0;
		for(int i=0;i<3;i++) {
			indices[20*i]=6*i; //0
			for(int j=1;j<=3; j++) {
				indices[20*i+j]=6*i+j; //right part (1,2,3)
				indices[20*i+12-j]=(6*i+18-j)%18; //right part (9,10,11)
			}
			for(int j=1;j<=2; j++) {
				indices[20*i+11+j]=17+4*i+j; //right part (12,13)
				indices[20*i+20-j]=20+(22-j+4*i)%12; //right part (18,19)
				//central part
				indices[20*i+3+j]=29+2*i+j; //4,5
				indices[20*i+6+j]=30+(6+2*i-j)%6; //7,8
				indices[20*i+13+j]=35+2*i+j; //14,15
				indices[20*i+15+j]=42+(6+2*i-j)%6; //17,18
			}
			indices[20*i+6]=48; //6
		}
		int tmp=60;
		//debug lines
		for(int i=0;i<18;i++) {
			indices[tmp++]=i;
			indices[tmp++]=(i+1)%18;
			if(i%3) { //"antennas"
				indices[tmp++]=i;
				indices[tmp++]=18+_i;
				_i++;
			}
			else if(i%6) { //"antennas"
				indices[tmp++]=i;
				indices[tmp++]=30+2*(i/6);
			}
		}
		for(int i=0;i<6;i++) {//antennas
			indices[tmp++]=30+i;
			indices[tmp++]=30+i+6;
			indices[tmp++]=30+i;
			indices[tmp++]=30+i+12;
		}
		for(int i=0;i<3;i++) {//lines
			indices[tmp++]=31+i;
			indices[tmp++]=30+i;
			indices[tmp++]=31+i;
			indices[tmp++]=48;
		}
		vertices.resize(49);
		glm::vec3 P[3][6]; //points of external triangle
		glm::vec3 N[3][4]; //antennas of triangle
		glm::vec3 Oc[3];
		glm::vec3 C[6]({}); //2/3 of central triangle
		glm::vec3 Cen{}; //central point
		glm::vec3 NC[2][6]({}); //-/+ from C[i] points
		//external triangle with antennas
		for(int i=0;i<3;i++) {
			xDiff = (order[2*i+1]-order[2*i])/3;
			yDiff = std::abs(4/xDiff) * (1-2*(std::max(order[2*i+1],order[2*i])==15));
			glm::vec3 T[3], O[2], S[3]; //S-centres of nearest Bézier curve segments, T-of second nearest, O-centres of adjacent T
			for(int j=0;j<3;j++) {
				T[j]=lerp(getPointPos(i,j,1),getPointPos(i,j+1,1),0.5f);
				S[j]=lerp(getPointPos(i,j,0),getPointPos(i,j+1,0),0.5f);
			}
			for(int j=0;j<2;j++) {
				O[j]=lerp(T[j],T[j+1],0.5f);
			}
			Oc[i]=lerp(O[0],O[1],0.5f);

			P[i][0]=getPointPos(i,0,0);
			P[i][1]=S[0];
			P[i][5]=S[2];
			P[i][2]=lerp(P[i][1],S[1],0.5f);
			P[i][4]=lerp(P[i][5],S[1],0.5f);
			P[i][3]=lerp(P[i][2],P[i][4],0.5f);
			//further
			N[i][0]=lerp(P[i][1],T[0],-1.f);
			N[i][1]=lerp(P[i][2],O[0],-1.f);
			N[i][2]=lerp(P[i][4],O[1],-1.f);
			N[i][3]=lerp(P[i][5],T[2],-1.f);
			//central triangle
			C[2*i]=lerp(P[i][3],Oc[i],-1.f);
		}

		//TODO - better central triangle
		Cen=(C[0]+C[2]+C[4])/3.f;
		for(int i=0;i<3;i++) {
			C[2*i+1]=lerp(C[2*i],Cen,0.5f);
		}
		for(int i=0;i<3;i++) {
			//central antennas
			glm::vec3 g0=P[i][2]-P[i][3];
			glm::vec3 g2=(Cen-C[(2*i+3)%6]+C[(2*i+5)%6]-Cen)*.5f;
			glm::vec3 g1=(g0+g2)*.5f;
			glm::vec3 gp2 = bezier2(2.f/3.f,g0,g1,g2);
			glm::vec3 gp1 = bezier2(1.f/3.f,g0,g1,g2);
			NC[0][2*i]=C[2*i]+gp1;
			NC[0][2*i+1]=C[2*i+1]+gp2;
			NC[1][2*i]=C[2*i]-gp1;
			NC[1][2*i+1]=C[2*i+1]-gp2;
		}

		//set vertices
		for(int i=0;i<3;i++) {
			for(int j=0;j<6;j++)
				vertices[6*i+j]=P[i][j];
			for(int j=0;j<4;j++)
				vertices[18+4*i+j]=N[i][j];
		}
		for(int i=0;i<6;i++) {
			vertices[30+i]=C[i];
			vertices[36+i]=NC[0][i];
			vertices[42+i]=NC[1][i];
		}
		vertices[48]=Cen;


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
	glm::vec3 GregoryPatch::getPointPos(int seg, bool isFurther) {
		return objectArray[segments[seg%3]->pointIndices[order[2*seg+isFurther]]].getPosition();
	}
	glm::vec3 GregoryPatch::getPointPos(int seg, int x, int y) {
		int b=order[2*seg]+y*yDiff+x*xDiff;
		return objectArray[segments[seg]->pointIndices[b]].getPosition();
	}
}// namespace bf
