//
// Created by kamil-hp on 06.05.23.
//

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
        : BezierSurfaceCommon(oArray, objName, c) {isC2=true;}

bf::BezierSurface2::BezierSurface2(bf::ObjectArray &oArray, const bf::Cursor &c) :
    BezierSurfaceCommon(oArray, c) {isC2=true;}

std::vector<std::vector<bf::pArray>> bf::BezierSurface2::generatePoints(const glm::vec2 &totalSize) {
    isC2 = true;
    auto P = static_cast<int>(objectArray.size());
    std::vector<std::vector<pArray> > pointIndices;
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
        for (int i = 0; i <= segs.y+3; i++) {
            for (int j = 0; j < segs.x; j++) {
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
                segsRow.emplace_back();
                for (int k = 0; k < 4; k++) {
                    for (int l = 0; l < 4; l++) {
                        segsRow.back()[4 * k + l] = P + (j + l)%S + S * (i + k);
                    }
                }
            }
            pointIndices.emplace_back(std::move(segsRow));
        }
    }
    return pointIndices;
}

using mat4d = glm::mat<4,4,double,glm::defaultp>;

constexpr mat4d M6 = mat4d({1,-3,3,-1},{4,0,-6,3},{1,3,3,-3},{0,0,0,1});

bf::vec3d multiplyPseudoMatrix(bf::vec4d left, bf::vec3d P[16], bf::vec4d right) {
	bf::vec3d tmp[4];
	for(int i=0;i<4;i++) {
		tmp[i]=bf::vec3d(0.0);
		for(int j=0;j<4;j++) {
			tmp[i]+=left[j]*P[4*i+j];
		}
	}
	auto ret=bf::vec3d(0.0);
	for(int i=0;i<4;i++) {
		ret+=tmp[i]*right[i];
	}
	return ret;
}

bf::vec3d bf::BezierSurface2::parameterFunction(double uf, double vf) const {
	auto&& [iu,iv,u,v]=setParameters(uf,vf);
	bf::vec4d uvec = {1.,u,u*u,u*u*u};
	bf::vec4d vvec = {1.,v,v*v,v*v*v};
	bf::vec4d pl = uvec*M6/6.;
	bf::vec4d pr = transpose(M6)*vvec/6.;
	bf::vec3d P[16];
	for(int i=0;i<16;i++) {
		P[i]=objectArray[segments[iv][iu].pointIndices[i]].getPosition();
	}
	return multiplyPseudoMatrix(pl, P, pr);
}
bf::vec3d bf::BezierSurface2::parameterGradientU(double uf, double vf) const {//TODO
	auto&& [iu,iv,u,v]=setParameters(uf,vf);
	bf::vec4d uvec = {0.,1.,2.*u,3.*u*u};
	bf::vec4d vvec = {1.,v,v*v,v*v*v};
	bf::vec4d pl = uvec*M6/6.0;
	bf::vec4d pr = transpose(M6)*vvec/6.0;
	bf::vec3d P[16];
	for(int i=0;i<16;i++) {
		P[i]=objectArray[segments[iv][iu].pointIndices[i]].getPosition();
	}
	return multiplyPseudoMatrix(pl, P, pr);
}
bf::vec3d bf::BezierSurface2::parameterGradientV(double uf, double vf) const {//TODO
	auto&& [iu,iv,u,v]=setParameters(uf,vf);
	bf::vec4d uvec = {1.,u,u*u,u*u*u};
	bf::vec4d vvec = {0.,1.,2.*v,3.*v*v};
	bf::vec4d pl = uvec*M6/6.;
	bf::vec4d pr = transpose(M6)*vvec/6.;
	bf::vec3d P[16];
	for(int i=0;i<16;i++) {
		P[i]=objectArray[segments[iv][iu].pointIndices[i]].getPosition();
	}
	return multiplyPseudoMatrix(pl, P, pr);
}
