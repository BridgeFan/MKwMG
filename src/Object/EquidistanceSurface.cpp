//
// Created by Kamil on 17.12.2024.
//

#include "EquidistanceSurface.h"
#include "MullingPathCreator.h"
#include "Solids/Torus.h"
#include "Util.h"
#include "Surfaces/BezierSurface2.h"
#include "Surfaces/BezierSurface0.h"
#include "Solids/Solid.h"
#include <glm/gtx/transform.hpp>

#include <algorithm>
#include <iostream>

namespace bf {
	double bf::EquidistanceSurface::scale = 0.0;
	EquidistanceSurface::EquidistanceSurface(Object &obj, double r) : object(obj) {
		updateScale(r);
		if (typeid(object) == typeid(bf::Torus)) {
			torus = dynamic_cast<bf::Torus *>(&object);
		}
		else if (typeid(object) == typeid(bf::BezierSurface2)) {
			if (!object.parameterWrappingU()) {
				isFinalAddedU=true;
			}
			if (!object.parameterWrappingV()) {
				if (!isFinalAddedU)
					isFinalAddedV=true;
				else
					isFinalAddedU=false;
			}
		}
		else if (typeid(object) == typeid(bf::BezierSurface0)) {
			//TODO - solve problems with singularity
			isFinalAddedU=true;
			isFinalAddedV=false;
			bezierSurface0 = dynamic_cast<bf::BezierSurface0*>(&object);
			/*if (!object.parameterWrappingU()) {
			}
			if (!object.parameterWrappingV()) {
				if (!isFinalAddedU)
					isFinalAddedV=true;
				else
					isFinalAddedU=false;
			}*/
		}
		if (isFinalAddedU) {
			singularUs = {0.1,0.1+object.getParameterMax().x};
		}
		if (isFinalAddedV) {
			singularVs = {0.1,0.1+object.getParameterMax().y};
		}
	}

	bf::vec4d EquidistanceSurface::processUV(double u, double v) const {
		auto M = getParameterMax();
		//clamp parameters
		if (bezierSurface0) {
			u = std::min(getParameterMax().x-1e-6,u);
		}
		if (object.parameterWrappingU()) {
			u = std::fmod(u, M.x);
			if (u < 0) u += M.x;
		} else
			u = std::clamp(u, 0.0, getParameterMax().x);
		if (object.parameterWrappingV()) {
			v = std::fmod(v, M.y);
			if (v < 0) v += M.y;
		} else
			v = std::clamp(v, 0.0, getParameterMax().y);
		//processing
		bf::vec4d ret = {u, v, 0.0, 0.0};
		if (isFinalAddedU) {
			if (u < 0.1) {
				ret.z = u;
				ret.x = 0.0;
			}
			else if (bezierSurface0) {
				if (u+0.1 > getParameterMax().x) {
					ret.x = getParameterMax().x-0.2-1e-6;
					ret.z = u - ret.x - 0.1;
				}
				else {
					ret.x -= 0.1;
				}
			}
			else if (u > object.getParameterMax().x + 0.1) {
				ret.x = object.getParameterMax().x;
				ret.z = u - ret.x - 0.1;
			} else
				ret.x -= 0.1;
		}
		if (isFinalAddedV) {
			if (v < 0.1) {
				ret.a = v;
				ret.y = 0.0;
			}
			else if (v > object.getParameterMax().y + 0.1) {
				ret.y = object.getParameterMax().y;
				ret.a = v - ret.y - 0.1;
			} else
				ret.y -= 0.1;
		}
		if (bezierSurface0) {
			ret.x = std::clamp(ret.x,1e-6,1.0-1e-6);
			ret.y = std::clamp(ret.y,1e-6,1.0-1e-6);
		}
		return ret;
	}
	const std::vector<double> &EquidistanceSurface::singularU() const {return singularUs;}
	const std::vector<double> &EquidistanceSurface::singularV() const {return singularVs;}

	void EquidistanceSurface::setMullingPathCreator(const MullingPathCreator &mpc) {
		scale = mpc.getMPCScale();
	}

	void EquidistanceSurface::draw(const ShaderArray &shaderArray) const {
		if (!torus && solid)
			solid->draw(shaderArray);
	}

	void EquidistanceSurface::onMergePoints(int, int) {}

	ShaderType EquidistanceSurface::getShaderType() const {
		return ShaderType::BasicShader;
	}

	bf::vec2d EquidistanceSurface::getParameterMin() const {
		return object.getParameterMin();
	}

	bf::vec2d EquidistanceSurface::getParameterMax() const {
		return object.getParameterMax()+0.2*bf::vec2d(isFinalAddedU,isFinalAddedV)+static_cast<double>(bezierSurface0!=nullptr)*bf::vec2d(-1.0,0.0);
	}

	bool EquidistanceSurface::parameterWrappingU() const {
		return object.parameterWrappingU() && !bezierSurface0;
	}

	bool EquidistanceSurface::parameterWrappingV() const {
		return object.parameterWrappingV() && !bezierSurface0;
	}

	bf::vec3d myRotate(const bf::vec3d& v0, const bf::vec3d& n, double t, bool isMin) {
		auto alpha = (isMin ? -1.0 : 1.0)*(t*PI*5.0);
		double const c = std::cos(alpha);
		double const s = std::sin(alpha);
		return c*v0 + (1.0-c)*n*glm::dot(v0,n) + s*glm::cross(n,v0);
	}

	bf::vec3d EquidistanceSurface::parameterFunction(double uf, double vf) const {
		auto vec = processUV(uf,vf);
		double u = vec.x;
		double v = vec.y;
		//special area for ends
		if (torus) {
			torus->smallRadius += scaledR;
			auto ret = object.parameterFunction(u,v);
			torus->smallRadius -= scaledR;
			return ret;
		}
		auto n = glm::cross(object.parameterGradientU(u,v), object.parameterGradientV(u,v));
		if (vec.z > 0.0) {
			auto axis = glm::normalize(object.parameterGradientV(u,v));
			double t=std::max(vec.z,vec.a);
			if (vec.x < 1e-5) {//begin
				t = 0.1-t;
			}
			t*=(1.0-0.8*(bezierSurface0!=nullptr));
			bool isMin = (vec.x<1e-5)==(bezierSurface0==nullptr);
			auto actualNormal = myRotate(-glm::normalize(n),axis,t,isMin)*scaledR;
			return object.parameterFunction(u,v)+actualNormal;
		}
		else if (vec.a > 0.0) {
			auto axis = glm::normalize(object.parameterGradientU(u,v));
			double t=std::max(vec.z,vec.a);
			if (vec.y < 1e-5) {//begin
				t = 0.1-t;
			}
			t*=(1.0-0.8*(bezierSurface0!=nullptr));
			bool isMin = (vec.y<1e-5)==(bezierSurface0==nullptr);
			auto actualNormal = myRotate(-glm::normalize(n),axis,t,isMin)*scaledR;
			return object.parameterFunction(u,v)+actualNormal;
		}
		return object.parameterFunction(u, v)-glm::normalize(n)*scaledR;
	}


	bf::vec3d calculateNormalizedDerivative(const bf::vec3d& f, const bf::vec3d& df) {
		//f*df - element by element multiplicatuib
		auto norm = glm::length(f);
		return df/norm - fastPow(1.0/norm,3)*(f*df);
	}

	bf::vec3d EquidistanceSurface::parameterGradientU(double uf, double vf) const {
    	if (torus) {
    		torus->smallRadius += scaledR;
    		auto ret = object.parameterGradientU(uf,vf);
    		torus->smallRadius -= scaledR;
    		return ret;
    	}
    	auto vec = processUV(uf,vf);
    	double u = vec.x;
    	double v = vec.y;
    	auto n = glm::cross(object.parameterGradientU(u,v), object.parameterGradientV(u,v));
    	auto pdn = glm::cross(object.parameterHesseUU(u,v), object.parameterGradientV(u,v))+glm::cross(object.parameterGradientU(u,v), object.parameterHesseUV(u,v));
		auto dn = calculateNormalizedDerivative(n,pdn);
		//auto dn = -(glm::cross(object.parameterHesseUU(u,v), object.parameterGradientV(u,v))+glm::cross(object.parameterGradientU(u,v), object.parameterHesseUV(u,v))) / norm * scaledR;
    	bf::vec3d modifier;
    	if (vec.z > 0.0) {
    		auto axis = glm::normalize(object.parameterGradientV(u,v));
    		n = normalize(n);
    		//TODO
    		double t=std::max(vec.z,vec.a);
    		if (u < 1e-5) {//begin
    			t = 0.1-t;
    		}
			t*=(1.0-0.8*(bezierSurface0!=nullptr));
    		bool isMin = (vec.x<1e-5)==(bezierSurface0==nullptr);
    		double alpha = PI*5.0*(isMin ? -1.0 : 1.0)*t;
    		double dAlpha = PI*5.0*(isMin ? -1.0 : 1.0)*(1.0-0.8*(bezierSurface0!=nullptr));
    		double c = std::cos(alpha);
    		double s = std::sin(alpha);
			return dAlpha * (s * (-axis+n*glm::dot(axis,n)) + c * glm::cross(n,axis));
    	}
    	else if (vec.a > 0.0) {
    		auto axis = object.parameterGradientV(u,v);
    		auto dAxis = calculateNormalizedDerivative(axis,object.parameterHesseUV(u,v));
    		double t=std::max(vec.z,vec.a);
    		if (v < 1e-5) {//begin
    			t = 0.1-t;
    		}
			t*=(1.0-0.8*(bezierSurface0!=nullptr));
    		bool isMin = (vec.y<1e-5)==(bezierSurface0==nullptr);
    		double alpha = PI*5.0*(isMin ? -1.0 : 1.0)*t;
    		double c = std::cos(alpha);
    		double uc = 1-c;
    		double s = std::sin(alpha);
    		modifier = -dn*scaledR*c;
    		//modifier = dAxis * c + uc * (dn*glm::dot(axis,n)+n*(glm::dot(dAxis,n)+glm::dot(axis,dn)))+ s * (glm::cross(dn,axis)+glm::cross(n,dAxis));
    	}
    	else
    		modifier = -dn*scaledR;
        return object.parameterGradientU(u, v)+modifier;
    }

	bf::vec3d EquidistanceSurface::parameterGradientV(double uf, double vf) const {
    	if (torus) {
    		torus->smallRadius += scaledR;
    		auto ret = object.parameterGradientV(uf,vf);
    		torus->smallRadius -= scaledR;
    		return ret;
    	}
    	auto vec = processUV(uf,vf);
    	double u = vec.x;
    	double v = vec.y;
		auto n = glm::cross(object.parameterGradientU(u,v), object.parameterGradientV(u,v));
		auto pdn = glm::cross(object.parameterHesseUV(u,v), object.parameterGradientV(u,v))+glm::cross(object.parameterGradientU(u,v), object.parameterHesseVV(u,v));
		auto dn = calculateNormalizedDerivative(n,pdn);
		bf::vec3d modifier;
    	if (vec.a > 0.0) {
    		auto axis = glm::normalize(object.parameterGradientU(u,v));
    		n = glm::normalize(n);
    		//TODO
    		double t=std::max(vec.z,vec.a);
    		if (vec.y < 1e-5) {//begin
    			t = 0.1-t;
    		}
			t*=(1.0-0.8*(bezierSurface0!=nullptr));
    		bool isMin = (vec.y<1e-5)==(bezierSurface0==nullptr);
    		double alpha = PI*5.0*(isMin ? -1.0 : 1.0)*t;
    		double dAlpha = PI*5.0*(isMin ? -1.0 : 1.0)*(1.0-0.8*(bezierSurface0!=nullptr));
    		double c = std::cos(alpha);
    		double s = std::sin(alpha);
    		return dAlpha * (s * (-axis+n*glm::dot(axis,n)) + c * glm::cross(n,axis));
    	}
    	else if (vec.z > 0.0) {
    		//auto axis = object.parameterGradientU(u,v);
    		//auto dAxis = calculateNormalizedDerivative(axis,object.parameterHesseUV(u,v));
    		double t=std::max(vec.z,vec.a);
    		if (vec.x < 1e-5) {//begin
    			t = 0.1-t;
    		}
			t*=(1.0-0.8*(bezierSurface0!=nullptr));
    		bool isMin = (vec.x<1e-5)==(bezierSurface0==nullptr);
    		double alpha = PI*5.0*(isMin ? -1.0 : 1.0)*t;
    		double c = std::cos(alpha);
    		double uc = 1-c;
    		double s = std::sin(alpha);
    		modifier = -dn*scaledR*c;
    		//modifier = dAxis * c + uc * (dn*glm::dot(axis,n)+n*(glm::dot(dAxis,n)+glm::dot(axis,dn)))+ s * (glm::cross(dn,axis)+glm::cross(n,dAxis));
    	}
    	else
    		modifier = -dn*scaledR;
    	return object.parameterGradientV(u, v)+modifier;
    }

    bf::Object &EquidistanceSurface::getObject() {
        return object;
    }

    const bf::Object &EquidistanceSurface::getObject() const {
		return object;
	}
	void EquidistanceSurface::updateScale(double R, bool isDrawn) {
    	scaledR = R / scale;
		if (!isDrawn) {return;}
    	//TODO - to remove
    	int divX = 40, divY=500;
    	solid.reset(new bf::DummySolid(""));
    	solid->vertices.reserve(3*divX*divY);
    	solid->indices.reserve(2*divX*divY);
    	for (int i=0;i<divX;i++) {
    		double u = getParameterMax().x*i/(divX-1.0);
    		for (int j=0;j<divY;j++) {
    			double v = getParameterMax().y*j/(divY-1.0);
    			auto func = parameterFunction(u,v);
    			auto size = solid->vertices.size();
    			solid->vertices.emplace_back(func);
    			if (i>0) {
    				solid->indices.emplace_back(size-divY);
    				solid->indices.emplace_back(size);
    			}
    			if (j>0) {
    				solid->indices.emplace_back(size-1);
    				solid->indices.emplace_back(size);
    			}
    		}
    	}
    	solid->setBuffers();
	}
} // bf