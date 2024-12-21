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
        : BezierSurfaceCommon(oArray, objName, c) {
}

bf::BezierSurface0::BezierSurface0(bf::ObjectArray &oArray, const bf::Cursor &c) : BezierSurfaceCommon(oArray,
                                                                                                            c) {}

std::vector<std::vector<bf::pArray>> bf::BezierSurface0::generatePoints(const glm::vec2 &totalSize) {
	isC2 = false;
	auto P = static_cast<int>(objectArray.size());
	std::vector<std::vector<pArray>> pointIndices;
	//generate points
	if (!isWrappedX) {
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
	} else {
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
						segsRow.back()[4 * k + l] = P + (3 * j + l) % S + S * (3 * i + k);
					}
				}
			}
			pointIndices.emplace_back(std::move(segsRow));
		}
	}
	return pointIndices;
}

bf::vec3d bf::BezierSurface0::parameterFunction(double u, double v, int iu, int iv, const std::function<bf::vec4d(double)> &buFunc, const std::function<bf::vec4d(double)> &bvFunc) const {
	// Basis functions for u and v
	bf::vec4d bu = buFunc(u);
	bf::vec4d bv = bvFunc(v);
	auto M=BezierSurfaceCommon::getParameterMax();
	int Mx=static_cast<int>(M.x);
	int My=static_cast<int>(M.y);
	iu=iu%Mx; if(iu<0) iu+=Mx;
	iv=iv%Mx; if(iv<0) iv+=My;
	bf::vec3d p(.0f);
	for(int i=0;i<4;i++) {
		for(int j=0;j<4;j++) {
			auto pos=objectArray[segments[iv][iu].pointIndices[4*j+i]].getPosition();
			bf::vec3d pos2 = {pos.x,pos.y,pos.z};
			p += pos2*bu[i]*bv[j];
		}
	}
	return p;
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

bf::vec4d basisFunctionsDD(double t) {
	double t1 = (1.0 - t);
	bf::vec4d b;
	// Bernstein polynomials (derivative)
	b[0] = 6.0 * t1;
	b[1] = 6.0 * (3.0*t-2.0);
	b[2] = 6.0 * (1.0-3.0*t);
	b[3] = 6.0 * t;
	return b;
}



bf::vec3d bf::BezierSurface0::parameterFunction(double u, double v, int iu, int iv) const {
	return parameterFunction(u,v,iu,iv, basisFunctions, basisFunctions);
}
bf::vec3d bf::BezierSurface0::parameterGradientU(double u, double v, int iu, int iv) const {
	return parameterFunction(u,v,iu,iv, basisFunctionsD, basisFunctions);
}
bf::vec3d bf::BezierSurface0::parameterGradientV(double u, double v, int iu, int iv) const {
	return parameterFunction(u,v,iu,iv, basisFunctions, basisFunctionsD);
}
bf::vec3d bf::BezierSurface0::parameterHesseUU(double u, double v, int iu, int iv) const {
	return parameterFunction(u,v,iu,iv, basisFunctionsDD, basisFunctions);
}
bf::vec3d bf::BezierSurface0::parameterHesseUV(double u, double v, int iu, int iv) const {
	return parameterFunction(u,v,iu,iv, basisFunctionsD, basisFunctionsD);
}
bf::vec3d bf::BezierSurface0::parameterHesseVV(double u, double v, int iu, int iv) const {
	return parameterFunction(u, v, iu, iv, basisFunctions, basisFunctionsDD);
}
std::vector<double> bf::BezierSurface0::initSingularU() const {
	bool loop = !parameterWrappingU();
	std::vector<double> u;
	u.reserve(getParameterMax().x);
	for (int i=0+loop;i<getParameterMax().x-loop;i++)
		u.emplace_back(i);
	return u;
}
std::vector<double> bf::BezierSurface0::initSingularV() const {
	bool loop = !parameterWrappingV();
	std::vector<double> v;
	v.reserve(getParameterMax().y);
	for (int i = 0 + loop; i <= getParameterMax().y - loop; i++)
		v.emplace_back(i);
	return v;
}
void bf::BezierSurface0::initSegments(std::vector<std::vector<std::string>> &&segmentNames, std::vector<std::vector<glm::vec<2, int>>> &&segmentSamples, std::vector<std::vector<pArray>> &&pointIndices) {
	BezierSurfaceCommon::initSegments(std::move(segmentNames), std::move(segmentSamples), std::move(pointIndices));
	singularUs = initSingularU();
	singularVs = initSingularV();

}
const std::vector<double>& bf::BezierSurface0::singularU() const {return singularUs;}
const std::vector<double>& bf::BezierSurface0::singularV() const {return singularVs;}
constexpr double EPS=0.0001;

bf::vec3d bf::BezierSurface0::parameterFunction(double ud, double vd) const {
	return setFunc(ud, vd, [this](double u, double v, int iu, int iv) {return this->parameterFunction(u,v,iu,iv);});
}
bf::vec3d bf::BezierSurface0::parameterGradientU(double ud, double vd) const {
	return setFunc(ud, vd, [this](double u, double v, int iu, int iv) {return this->parameterGradientU(u,v,iu,iv);});
}
bf::vec3d bf::BezierSurface0::parameterGradientV(double ud, double vd) const {
	return setFunc(ud, vd, [this](double u, double v, int iu, int iv) {return this->parameterGradientV(u,v,iu,iv);});
}
bf::vec3d bf::BezierSurface0::parameterHesseUU(double ud, double vd) const {
	return setFunc(ud, vd, [this](double u, double v, int iu, int iv) {return this->parameterHesseUU(u,v,iu,iv);});
}
bf::vec3d bf::BezierSurface0::parameterHesseUV(double ud, double vd) const {
	return setFunc(ud, vd, [this](double u, double v, int iu, int iv) {return this->parameterHesseUV(u,v,iu,iv);});
}
bf::vec3d bf::BezierSurface0::parameterHesseVV(double ud, double vd) const {
	return setFunc(ud, vd, [this](double u, double v, int iu, int iv) {return this->parameterHesseVV(u,v,iu,iv);});
}

bf::vec3d bf::BezierSurface0::setFunc(double uf, double vf, const std::function<bf::vec3d(double, double, int, int)>& func) const {
	auto &&[iu, iv, u, v] = setParameters(uf, vf);
	auto p = func(u, v, iu, iv);
	if ((iu > 0 && iu < getParameterMax().x) || BezierSurfaceCommon::parameterWrappingU()) {//no extreme
		if (u < EPS)
			return lerp(func(u + 1.0, v, iu - 1, iv), p, .5 + u / (2. * EPS));
		if (u > 1.0 - EPS)
			return lerp(func(u - 1.0, v, iu + 1, iv), p, .5 + (1.0 - u) / (2. * EPS));
	}
	if ((iv > 0 && iv < getParameterMax().y) || BezierSurfaceCommon::parameterWrappingV()) {//no extreme
		if (v < EPS)
			return lerp(func(u, v + 1.0, iu, iv - 1), p, .5 + v / (2. * EPS));
		if (v > 1.0 - EPS)
			return lerp(func(u, v - 1.0, iu, iv + 1), p, .5 + (1.0 - v) / (2. * EPS));
	}
	return p;

}