//
// Created by kamil-hp on 28.08.23.
//

#include "IntersectionObject.h"
#include "ConfigState.h"
#include "Curves/BezierCurveInter.h"
#include "ImGui/ImGuiUtil.h"
#include "ImGui/imgui_include.h"
#include "ObjectArray.h"
#include "Shader/ShaderArray.h"
#include "Gizmos/MultiCursor.h"
#include "Util.h"
#include <GL/glew.h>
#include <functional>
#include <iostream>
#include <random>

std::random_device rd;

bf::ObjectArray* bf::IntersectionObject::objectArray = nullptr;
std::pair<bf::Object*, int> bf::IntersectionObject::convertToCurve() {
	if(!obj1) return {nullptr, 0};
	unsigned beginIndex = objectArray->size();
	objectArray->clearSelection();
	//generate points
	for(const auto& p: intersectionPoints) {
		auto pos = obj1->parameterFunction(p.x,p.y);
		if(obj2) {
			pos = (pos + obj2->parameterFunction(p.z,p.w)) * 0.5;
		}
		objectArray->add<bf::Point>(pos);
	}
	auto curve = new bf::BezierCurveInter(*objectArray);
	for(unsigned i=beginIndex;i<objectArray->size();i++) {
		curve->addPoint(i);
	}
	if(isLooped && beginIndex<objectArray->size()) {
		curve->addPoint(beginIndex);
	}
	for(unsigned i=0;i<beginIndex;i++) {
		if(objectArray->getPtr(i)==this)
			return {curve, i};
	}
	return {curve, 0};
}

bf::IntersectionObject::IntersectionObject(bf::ObjectArray& oArray) {
	obj1=obj2=nullptr;
	if(!objectArray) {
		initObjectArray(oArray);
	}
	for(unsigned i=0;i<objectArray->size();i++) {
		if(objectArray->isActive(i) && (*objectArray)[i].isIntersectable()) {
			if(!obj1) {
				obj1 = &(*objectArray)[i];
				obj1->indestructibilityIndex++;
			}
			else if(!obj2) {
				obj2 = &(*objectArray)[i];
				obj2->indestructibilityIndex++;
				break;
			}
		}
	}
	isInitPhase=true;
	if(!obj1) {
		toRemove=true;
	}
}
void bf::IntersectionObject::ObjectGui() {
	bf::imgui::checkChanged("Intersection name", name);
	if(isInitPhase) {
		if(!obj1) {
			ImGui::Text("Selected no intersectable objects");
			if (ImGui::Button("OK")) {
				isInitPhase = false;
				objectArray->isForcedActive = false;
			}
		}
		else {
			static bool isCursor;
			static float precision=.01f;
			ImGui::Checkbox("Use cursor", &isCursor);
			bf::imgui::checkChanged("Precision", precision, .001f, 10.f, .001f, .05f);
			if (ImGui::Button("Confirm")) {
				findIntersection(isCursor);
				isInitPhase = false;
				objectArray->isForcedActive = false;
			}
		}
	}
	else {
		if (ImGui::Button("Convert to curve")) {
			auto curve = convertToCurve();
			if(!curve.first) return;
			auto thisObject = std::move(objectArray->objects[curve.second]).first;
			objectArray->objects[curve.second].first.reset(curve.first);
		}
	}
}
bf::IntersectionObject::~IntersectionObject() {
	if(obj1 && obj1->indestructibilityIndex>0)
		obj1->indestructibilityIndex--;
	if(obj2 && obj2->indestructibilityIndex>0)
		obj2->indestructibilityIndex--;
}
void bf::IntersectionObject::draw(const bf::ShaderArray& shaderArray) const {
	//Solid::draw(shaderArray);
	if(shaderArray.getActiveIndex()!=bf::ShaderType::BasicShader || indices.empty()) return;
	auto color = shaderArray.getColor();
	shaderArray.setColor(255,255,0);
	glBindVertexArray(VAO);
	shaderArray.getActiveShader().setMat4("model", glm::mat4(1.f));
	shaderArray.getActiveShader().setFloat("pointSize", 4.f*configState->pointRadius);
	glDrawElements(GL_POINTS, indices.size(), GL_UNSIGNED_INT,   // type
				   reinterpret_cast<void*>(0));         // element array buffer offset
	glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT,   // type
				   reinterpret_cast<void*>(0));         // element array buffer offset
	shaderArray.setColor(color);
}
bf::ShaderType bf::IntersectionObject::getShaderType() const {
	return bf::ShaderType::MultipleShaders;
}

void clampParams(bf::vec2d& x, const bf::Object& obj) {
	for(int i=0;i<2;i++) {
		if(!obj.parameterWrapping()[i]) x[i]=std::clamp(x[i],obj.getParameterMin()[i],obj.getParameterMax()[i]);
		else x[i]=std::fmod(x[i],obj.getParameterMax()[i]);
		if(x[i]<0.0) x[i]+=obj.getParameterMax()[i];
	}
}

void clampParams(bf::vec4d& x, const bf::Object& o1, const bf::Object& o2) {
	bf::vec2d x1={x.x,x.y},x2={x.z,x.w};
	clampParams(x1,o1);
	clampParams(x2,o2);
	x={x1.x, x1.y, x2.x,x2.y};
}

bf::vec2d findBeginningPoint(const bf::Object& obj, const glm::vec3& pos) {
	double minDist=std::numeric_limits<double>::max();;
	static std::uniform_real_distribution<> dis(0.,1.);
	//init
	bf::vec2d x, xn;
	for(int i=0;i<50;i++) {
		bf::vec2d tp;
		tp.x=lerp(obj.getParameterMin()[0],obj.getParameterMax()[0],dis(rd));
		tp.y=lerp(obj.getParameterMin()[1],obj.getParameterMax()[1],dis(rd));
		double dist=bf::IntersectionObject::distance(obj,pos,{tp.x,tp.y});
		if(dist<minDist) {
			minDist=dist;
			x=tp;
		}
	}
	xn=-x;
	constexpr double epsDist=1e-8;
	constexpr double eps=1e-6;
	double a=0.1;
	int N=0;
	bf::vec2d df;
	//find point
	while(N<1000 && a>eps && bf::IntersectionObject::distance(obj,pos,x)>epsDist && glm::dot(df,df)>eps && glm::dot(xn-x,xn-x)>eps*eps) {
		//TODO - proper stop
		x=xn;
		//finding antigradient
		df=bf::vec4d(.0);
		for(int i=0;i<3;i++) {//x,y,z
			auto f = obj.parameterFunction(x[0],x[1])[i];
			double g = pos[i];
			auto dfu = obj.parameterGradientU(x[0],x[1])[i];
			auto dfv = obj.parameterGradientV(x[0],x[1])[i];
			df[0]+=2.f*(f-g)*dfu;
			df[1]+=2.f*(f-g)*dfv;
		}
		//finding step
		xn=x-df*a;
		clampParams(xn, obj);
		while(bf::IntersectionObject::distance(obj,pos,xn)>bf::IntersectionObject::distance(obj,pos,x) && a>eps) { //too big step
			a*=.5f;
			xn=x-df*a;
			clampParams(xn, obj);
		}
		N++;
	}
	std::cout << N << " " << x.x << " " << x.y << "\n";
	return x;
}

void bf::IntersectionObject::findIntersection(bool isCursor) {
	vertices.clear();
	indices.clear();
	auto* o = obj2 ? obj2 : obj1;
	auto pos2 = transform.position;
	//no point chosen
	bf::vec4d x0;
	//begin data to algorithm
	constexpr double epsDist=1e-8;
	constexpr double eps=1e-6;
	double a=0.1;
	int N=0;
	if(isCursor) { //cursor begin
		std::cout << pos2.x << " " << pos2.y << " " << pos2.z << "\n";
		//find beginning point - u,v
		auto t1 = findBeginningPoint(*obj1, pos2);
		auto t2 = findBeginningPoint(*o, pos2);
		x0={t1.x,t1.y,t2.x,t2.y};
		vertices.emplace_back(obj1->parameterFunction(x0[0],x0[1]));
		vertices.emplace_back(o->parameterFunction(x0[2],x0[3]));
		indices.emplace_back(vertices.size()-2);
		indices.emplace_back(vertices.size()-1);
		std::cout << bf::IntersectionObject::distance(*obj1, *o, x0) << "\n";
	}
	else {
		double minDist=std::numeric_limits<double>::max();;
		static std::uniform_real_distribution<> dis(0.,1.);
		for(int i=0;i<50;i++) {
			bf::vec4d tp;
			tp.x=lerp(obj1->getParameterMin()[0],obj1->getParameterMax()[0],dis(rd));
			tp.y=lerp(obj1->getParameterMin()[1],obj1->getParameterMax()[1],dis(rd));
			tp.z=lerp(o->getParameterMin()[0],o->getParameterMax()[0],dis(rd));
			tp.w=lerp(o->getParameterMin()[1],o->getParameterMax()[1],dis(rd));
			double dist= distance(*obj1,*o,tp);
			if(dist<minDist) {
				minDist=dist;
				x0=tp;
			}
		}
	}
	vertices.emplace_back(obj1->parameterFunction(x0[0],x0[1]));
	vertices.emplace_back(o->parameterFunction(x0[2],x0[3]));
	indices.emplace_back(vertices.size()-2);
	indices.emplace_back(vertices.size()-1);
	//2 - minimizing distance
	bf::vec4d x=x0;
	clampParams(x0, *obj1, *o);
	bf::vec4d xn=-x0;
	std::cout << a << ": " << xn.x << " " << xn.y << " " << xn.z << " " << xn.w << "\n";
	bf::vec4d df{100000.};
	N=0;
	a=0.1;
	while(N<10000 && a>eps && distance(*obj1,*o,xn)>epsDist && glm::dot(df,df)>eps && glm::dot(xn-x,xn-x)>eps*eps) {
		//TODO - proper stop
		x=xn;
		//finding antigradient
		df=glm::vec4(.0f);
		for(int i=0;i<3;i++) {//x,y,z
			auto f = obj1->parameterFunction(x[0],x[1])[i];
			auto g = o->parameterFunction(x[2],x[3])[i];
			auto dfu = obj1->parameterGradientU(x[0],x[1])[i];
			auto dfv = obj1->parameterGradientV(x[0],x[1])[i];
			auto dgw = o->parameterGradientU(x[2],x[3])[i];
			auto dgt = o->parameterGradientV(x[2],x[3])[i];
			df[0]+=2.f*(f-g)*dfu;
			df[1]+=2.f*(f-g)*dfv;
			df[2]+=-2.f*(f-g)*dgw;
			df[3]+=-2.f*(f-g)*dgt;
		}
		//finding step
		xn=x-df*a;
		clampParams(xn, *obj1, *o);
		while(distance(*obj1,*o,xn)>distance(*obj1,*o,x) && a>eps) { //too big step
			a*=.5f;
			xn=x-df*a;
			clampParams(xn, *obj1, *o);
		}
		vertices.emplace_back(obj1->parameterFunction(x[0],x[1]));
		vertices.emplace_back(o->parameterFunction(x[2],x[3]));
		indices.emplace_back(vertices.size()-2);
		indices.emplace_back(vertices.size()-1);
		N++;
	}
	vertices.emplace_back(obj1->parameterFunction(x[0],x[1]));
	vertices.emplace_back(o->parameterFunction(x[2],x[3]));
	indices.emplace_back(vertices.size()-2);
	indices.emplace_back(vertices.size()-1);
	std::cout << N << " " << distance(*obj1, *o, x) << "\n";
	if(distance(*obj1,*o,x)>eps) {
		//no intersection
		toRemove=true;
		return;
	}
	intersectionPoints.emplace_back(x);
	setBuffers();
	//TODO - go through curves

}
double bf::IntersectionObject::distance(const bf::Object &o1, const bf::Object &o2, const bf::vec4d& t) {
	double ret=.0;
	for(int i=0;i<3;i++) {
		ret+=fastPow(o1.parameterFunction(t.x,t.y)[i]-o2.parameterFunction(t.z,t.w)[i], 2);
	}
	return ret;
}
double bf::IntersectionObject::distance(const bf::Object &o1, const bf::vec3d &p, const bf::vec2d& t) {
	double ret=.0;
	for(int i=0;i<3;i++) {
		ret+=fastPow(o1.parameterFunction(t.x,t.y)[i]-p[i], 2);
	}
	return ret;
}
bool bf::IntersectionObject::postInit() {
	return toRemove;
}
bool bf::IntersectionObject::isMovable() const {
	return isInitPhase;
}
bool bf::IntersectionObject::isIntersectable() const {
	return false;
}
void bf::IntersectionObject::onMergePoints(int p1, int p2) {}
