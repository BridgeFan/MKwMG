//
// Created by kamil-hp on 06.05.23.
//

#include "ImGui/imgui_include.h"
#include "BezierSurface0.h"
#include "Shader/ShaderArray.h"
#include "ImGui/ImGuiUtil.h"
#include "Object/ObjectArray.h"
#include "src/Gizmos/Cursor.h"
#include "Util.h"
#include "Object/Point.h"

int bf::BezierSurface0::_index = 1;


bf::BezierSurface0::BezierSurface0(bf::ObjectArray &oArray, const std::string &objName, const bf::Cursor &c)
        : BezierSurfaceCommon(oArray, objName, c) {}

bf::BezierSurface0::BezierSurface0(bf::ObjectArray &oArray, const bf::Cursor &c) : BezierSurfaceCommon(oArray,
                                                                                                            c) {}

std::vector<std::vector<bf::pArray>> bf::BezierSurface0::generatePoints(const glm::vec2 &totalSize) {
    isC2 = false;
    auto P = static_cast<int>(objectArray.size());
    std::vector<std::vector<pArray> > pointIndices;
    //generate points
    if(!isWrappedX) {
        float dx = totalSize.x / static_cast<float>(segs.x) / 3.f;
        float dy = totalSize.y / static_cast<float>(segs.y) / 3.f;
        for (int i = 0; i <= 3 * segs.y; i++) {
            for (int j = 0; j <= 3 * segs.x; j++) {
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
        float dt = 2.f * PI / (3.f * static_cast<float>(segs.x));
        float dy = totalSize.y / static_cast<float>(segs.y) / 3.f;
        for (int i = 0; i <= 3 * segs.y; i++) {
            for (int j = 0; j < 3 * segs.x; j++) {
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
			int S = segs.x * 3;
			std::vector<pArray> segsRow;
			for (int j = 0; j < segs.x; j++) {
				segsRow.emplace_back();
				for (int k = 0; k < 4; k++) {
					for (int l = 0; l < 4; l++) {
						segsRow.back()[4 * k + l] = P + (3 * j + l)%S + S * (3 * i + k);
					}
				}
			}
			pointIndices.emplace_back(std::move(segsRow));
		}
    }
    return pointIndices;
}

bf::vec4d basisFunctions(double t) {
	double t1 = (1.0f - t);
	double t12 = t1 * t1;
	bf::vec4d b;
	// Bernstein polynomials
	b[0] = t12 * t1;
	b[1] = 3.0 * t12 * t;
	b[2] = 3.0 * t1 * t * t;
	b[3] = t * t * t;
	return b;
}

bf::vec4d basisFunctionsD(double t) {
	double t1 = (1.0 - t);
	bf::vec4d b;
	// Bernstein polynomials (derivative)
	b[0] = -3.0 * t1 * t1;
	b[1] = 3.0 * (t-1.0) * (3.0*t-1.0);
	b[2] = 3.0 * t  * (2.0-3.0*t);
	b[3] = 3.0 * t * t;
	return b;
}

bf::vec3d bf::BezierSurface0::parameterFunction(double uf, double vf) const {
	auto&& [iu,iv,u,v]=setParameters(uf,vf);
	// Basis functions for u and v
	bf::vec4d bu = basisFunctions(u);
	bf::vec4d bv = basisFunctions(v);
	bf::vec3d p(.0f);
	for(int i=0;i<4;i++) {
		for(int j=0;j<4;j++) {
			auto pos=objectArray[segments[iv][iu].pointIndices[4*i+j]].getPosition();
			bf::vec3d pos2 = {pos.x,pos.y,pos.z};
			p += pos2*bu[i]*bv[j];
		}
	}
	return p;
}
bf::vec3d bf::BezierSurface0::parameterGradientU(double uf, double vf) const {
	auto&& [iu,iv,u,v]=setParameters(uf,vf);
	// Basis functions for u and v
	bf::vec4d bu = basisFunctionsD(u);
	bf::vec4d bv = basisFunctions(v);
	bf::vec3d p(.0f);
	for(int i=0;i<4;i++) {
		for(int j=0;j<4;j++) {
			auto pos=objectArray[segments[iv][iu].pointIndices[4*i+j]].getPosition();
			bf::vec3d pos2 = {pos.x,pos.y,pos.z};
			p += pos2*bu[i]*bv[j];
		}
	}
	return p;
}
bf::vec3d bf::BezierSurface0::parameterGradientV(double uf, double vf) const {
	auto&& [iu,iv,u,v]=setParameters(uf,vf);
	// Basis functions for u and v
	bf::vec4d bu = basisFunctions(u);
	bf::vec4d bv = basisFunctionsD(v);
	bf::vec3d p(.0f);
	for(int i=0;i<4;i++) {
		for(int j=0;j<4;j++) {
			auto pos=objectArray[segments[iv][iu].pointIndices[4*i+j]].getPosition();
			bf::vec3d pos2 = {pos.x,pos.y,pos.z};
			p += pos2*bu[i]*bv[j];
		}
	}
	return p;
}
