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


constexpr glm::mat4 M6 = glm::mat4({1,-3,3,-1},{4,0,-6,3},{1,3,3,-3},{0,0,0,1});

glm::vec3 multiplyPseudoMatrix(glm::vec4 left, glm::vec3 P[16], glm::vec4 right) {
	glm::vec3 tmp[4];
	for(int i=0;i<4;i++) {
		tmp[i]=glm::vec3(0.0f);
		for(int j=0;j<4;j++) {
			tmp[i]+=left[j]*P[4*i+j];
		}
	}
	glm::vec3 ret=glm::vec3(.0f);
	for(int i=0;i<4;i++) {
		ret+=tmp[i]*right[i];
	}
	return ret;
}

glm::vec3 bf::BezierSurface2::parameterFunction(float uf, float vf) const {
	glm::vec4 param = clampParam(uf,vf,1.f);
	int iu=int(param.z);
	int iv=int(param.w);
	float uV = param.x;
	float vV = param.y;
	glm::vec4 uvec = {1.f,uV,uV*uV,uV*uV*uV};
	glm::vec4 vvec = {1.f,vV,vV*vV,vV*vV*vV};
	glm::vec4 pl = uvec*M6/6.f;
	glm::vec4 pr = transpose(M6)*vvec/6.f;
	glm::vec3 P[16];
	for(int i=0;i<16;i++) {
		P[i]=objectArray[segments[iu][iv].pointIndices[i]].getPosition();
	}
	return multiplyPseudoMatrix(pl, P, pr);
}
glm::vec3 bf::BezierSurface2::parameterGradientU(float uf, float vf) const {//TODO
	glm::vec4 param = clampParam(uf,vf,1.f);
	int iu=int(param.z);
	int iv=int(param.w);
	float uV = param.x;
	float vV = param.y;
	glm::vec4 uvec = {0.f,1.f,2.f*uV,3.f*uV*uV};
	glm::vec4 vvec = {1.f,vV,vV*vV,vV*vV*vV};
	glm::vec4 pl = uvec*M6/6.f;
	glm::vec4 pr = transpose(M6)*vvec/6.f;
	glm::vec3 P[16];
	for(int i=0;i<16;i++) {
		P[i]=objectArray[segments[iu][iv].pointIndices[i]].getPosition();
	}
	return multiplyPseudoMatrix(pl, P, pr);
}
glm::vec3 bf::BezierSurface2::parameterGradientV(float uf, float vf) const {//TODO
	glm::vec4 param = clampParam(uf,vf,1.f);
	int iu=int(param.z);
	int iv=int(param.w);
	float uV = param.x;
	float vV = param.y;
	glm::vec4 uvec = {1.f,uV,uV*uV,uV*uV*uV};
	glm::vec4 vvec = {0.f,1.f,2.f*vV,3.f*vV*vV};
	glm::vec4 pl = uvec*M6/6.f;
	glm::vec4 pr = transpose(M6)*vvec/6.f;
	glm::vec3 P[16];
	for(int i=0;i<16;i++) {
		P[i]=objectArray[segments[iu][iv].pointIndices[i]].getPosition();
	}
	return multiplyPseudoMatrix(pl, P, pr);
}
