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

std::pair<bf::Object*, int> bf::IntersectionObject::convertToCurve() {
	if(!obj1) return {nullptr, 0};
	unsigned beginIndex = objectArray.size();
	objectArray.clearSelection();
	//generate points
	for(const auto& p: intersectionPoints) {
		auto pos = obj1->parameterFunction(p.x,p.y);
		if(obj2) {
			pos = (pos + obj2->parameterFunction(p.z,p.w)) * 0.5;
		}
		objectArray.add<bf::Point>(pos);
	}
	auto curve = new bf::BezierCurveInter(objectArray);
	for(unsigned i=beginIndex;i<objectArray.size();i++) {
		curve->addPoint(i);
	}
	curve->addPoint(beginIndex);
	for(unsigned i=0;i<beginIndex;i++) {
		if(objectArray.getPtr(i)==this)
			return {curve, i};
	}
	return {curve, 0};
}

bf::IntersectionObject::IntersectionObject(bf::ObjectArray &array) : ObjectArrayListener(array) {
	obj1=obj2=nullptr;
	for(unsigned i=0;i<objectArray.size();i++) {
		if(objectArray.isActive(i) && objectArray[i].isIntersectable()) {
			if(!obj1) {
				obj1 = &objectArray[i];
				obj1->indestructibilityIndex++;
			}
			else if(!obj2) {
				obj2 = &objectArray[i];
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
				objectArray.isForcedActive = false;
			}
		}
		else {
			static bool isCursor;
			static float precision=.01f;
			ImGui::Checkbox("Use cursor", &isCursor);
			bf::imgui::checkChanged("Precision", precision, .001f, 10.f, .001f, .05f);
			if (ImGui::Button("Confirm")) {
				findIntersection(isCursor, precision);
				isInitPhase = false;
				objectArray.isForcedActive = false;
			}
		}
	}
	else {
		if (ImGui::Button("Convert to curve")) {
			auto curve = convertToCurve();
			if(!curve.first) return;
			auto thisObject = std::move(objectArray.objects[curve.second]).first;
			objectArray.objects[curve.second].first.reset(curve.first);
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
	glDrawArrays(GL_POINTS, 0, GL_UNSIGNED_INT);
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

//for numerical methods
constexpr double epsDist=1e-10;
constexpr double eps=1e-6;
constexpr double epsBig=1e-4;

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
			xn=tp;
		}
	}
	x=-xn;
	double a=0.1;
	int N=0;
	bf::vec2d df(1.);
	//find point
	while(N<1000 && a>eps && bf::IntersectionObject::distance(obj,pos,x)>epsDist && glm::dot(df,df)>epsDist && glm::dot(xn-x,xn-x)>epsDist) {
		//TODO - proper step
		x=xn;
		//finding antigradient
		df=bf::vec4d(.0);
		auto f = obj.parameterFunction(x[0],x[1]);
		auto dfu = obj.parameterGradientU(x[0],x[1]);
		auto dfv = obj.parameterGradientV(x[0],x[1]);
		for(int i=0;i<3;i++) {//x,y,z
			double g = pos[i];
			df[0]+=2.0*(f[i]-g)*dfu[i];
			df[1]+=2.0*(f[i]-g)*dfv[i];
		}
		//finding step
		xn=x-df*a;
		clampParams(xn, obj);
		while(bf::IntersectionObject::distance(obj,pos,xn)>bf::IntersectionObject::distance(obj,pos,x) && a>eps) { //too big step
			a*=.5;
			xn=x-df*a;
			clampParams(xn, obj);
		}
		N++;
	}
	x=xn;
	std::cout << N << " " << x.x << " " << x.y << "\n";
	std::cout << a << " " << glm::dot(df,df) << " " << glm::dot(xn-x,xn-x) << "\n";
	return x;
}

constexpr double movingAdditionWage = 0.1;

void bf::IntersectionObject::findIntersection(bool isCursor, double precision) {
	auto* o = obj2 ? obj2 : obj1;
	auto pos2 = transform.position;
	//no point chosen
	bf::vec4d x0;
	//begin data to algorithm
	double a=0.1;
	int N=0;
	if(isCursor) { //cursor begin
		std::cout << pos2.x << " " << pos2.y << " " << pos2.z << "\n";
		//find beginning point - u,v
		auto t1 = findBeginningPoint(*obj1, pos2);
		auto t2 = findBeginningPoint(*o, pos2);
		x0={t1.x,t1.y,t2.x,t2.y};
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
	//2 - minimizing distance
	clampParams(x0, *obj1, *o);
	bf::vec4d x=-x0;
	bf::vec4d xn=x0;
	std::cout << a << ": " << xn.x << " " << xn.y << " " << xn.z << " " << xn.w << "\nLooking for first point:\n";
	bf::vec4d df{100000.};
	N=0;
	a=0.1;
	if(o!=obj1)
	while(N<10000 && a>eps && distance(*obj1,*o,xn)>epsDist && glm::dot(df,df)>epsDist && glm::dot(xn-x,xn-x)>eps*eps) {
		//TODO - proper step
		x=xn;
		//finding antigradient
		df=glm::vec4(.0);
		auto f = obj1->parameterFunction(x[0],x[1]);
		auto g = o->parameterFunction(x[2],x[3]);
		auto dfu = obj1->parameterGradientU(x[0],x[1]);
		auto dfv = obj1->parameterGradientV(x[0],x[1]);
		auto dgw = o->parameterGradientU(x[2],x[3]);
		auto dgt = o->parameterGradientV(x[2],x[3]);
		for(int i=0;i<3;i++) {//x,y,z
			df[0]+=2.*(f[i]-g[i])*dfu[i];
			df[1]+=2.*(f[i]-g[i])*dfv[i];
			df[2]+=-2.*(f[i]-g[i])*dgw[i];
			df[3]+=-2.*(f[i]-g[i])*dgt[i];
		}
		//finding step
		xn=x-df*a;
		clampParams(xn, *obj1, *o);
		while(distance(*obj1,*o,xn)>distance(*obj1,*o,x) && a>eps) { //too big step
			a*=.5f;
			xn=x-df*a;
			clampParams(xn, *obj1, *o);
		}
		N++;
	}
	std::cout << N << " " << a << " " << glm::dot(df,df) << " " << glm::dot(xn-x,xn-x) << "\n";
	x=xn;
	std::cout << N << " " << distance(*obj1, *o, x) << "\n";
	if(distance(*obj1,*o,x)>epsBig) {
		std::cout << "No intersection found\n";
		toRemove=true;
		return;
	}
	std::cout << "March\n";
	intersectionPoints.emplace_back(x);
	setBuffers();
	//Go through
	//TODO - loop
	constexpr double eps2=1e-5;
	constexpr double eps2dist=1e-8;
	int M;
	bf::vec4d oldX;
	for(M=0;M<1000;M++) {
		//simple gradient is enough here
		auto y=x-intersectionPoints[0];
		if(M>=3 && glm::dot(y,y)<precision*precision) {
			break;
		}
		N = 0;
		a=precision*5.0;
		df=bf::vec4d(1.0);
		auto P0 = (obj1->parameterFunction(xn[0], xn[1]) + o->parameterFunction(xn[2], xn[3])) * 0.5;
		auto n1 = glm::cross(obj1->parameterGradientU(xn[0], xn[1]), obj1->parameterGradientV(xn[0], xn[1]));
		auto n2 = glm::cross(o->parameterGradientU(xn[2], xn[3]), o->parameterGradientV(xn[2], xn[3]));
		auto t = glm::normalize(glm::cross(n1, n2));
		auto P = obj1->parameterFunction(xn[0], xn[1]);
		auto Q = o->parameterFunction(xn[2], xn[3]);
		bf::vec3d Pn;
		if(M==0)
			xn = x + bf::vec4d(std::sqrt(3.0) * precision);
		else
			xn = lerp(oldX,xn,2.0);
		oldX=x;
		while (N < (M==0 ? 10000 : 1000) && a > eps2 && movDist(P, Q, P0, t, precision) > eps2dist && glm::dot(df, df) > epsDist /*&& glm::dot(xn - x, xn - x) > epsDist*/) {
			x = xn;
			//finding antigradient
			P = obj1->parameterFunction(x[0], x[1]);
			Q = o->parameterFunction(x[2], x[3]);
			auto dPu = obj1->parameterGradientU(x[0], x[1]);
			auto dPv = obj1->parameterGradientV(x[0], x[1]);
			auto dQw = o->parameterGradientU(x[2], x[3]);
			auto dQt = o->parameterGradientV(x[2], x[3]);
			auto dotek = glm::dot(P - P0, t) - precision;
			df = bf::vec4d(0.0);
			for (int i = 0; i < 3; i++) {//x,y,z
				df[0] += 2.0 * (P[i] - Q[i]) * dPu[i] + 2 * dotek * glm::dot(dPu, t)*movingAdditionWage;
				df[1] += 2.0 * (P[i] - Q[i]) * dPv[i] + 2 * dotek * glm::dot(dPv, t)*movingAdditionWage;
				df[2] += -2. * (P[i] - Q[i]) * dQw[i];
				df[3] += -2. * (P[i] - Q[i]) * dQt[i];
			}
			//finding step
			xn = x - df * a;
			clampParams(xn, *obj1, *o);
			Pn = obj1->parameterFunction(xn[0], xn[1]);
			auto Qn = o->parameterFunction(xn[2], xn[3]);
			while (movDist(Pn, Qn, P0, t, precision) > movDist(P, Q, P0, t, precision) && a > eps2) {//too big step
				a *= .9;
				xn = x - df * a;
				clampParams(xn, *obj1, *o);
				Pn = obj1->parameterFunction(xn[0], xn[1]);
				Qn = o->parameterFunction(xn[2], xn[3]);
			}
			//finding step - steepest descent
			/*double l=0.0,r=a,s;
			for(double dif=a;dif>eps2;dif*=0.5) {
				s=(l+r)*0.5;
				xn = x - df * a;
				clampParams(xn, *obj1, *o);
				Pn = obj1->parameterFunction(xn[0], xn[1]);
				auto Qn = o->parameterFunction(xn[2], xn[3]);

			}*/
			N++;
		}
		P = obj1->parameterFunction(xn[0], xn[1]);
		Q = o->parameterFunction(xn[2], xn[3]);
		std::cout << N << "   \t" << glm::distance(P,P0) << "\t" << glm::dot(P-Q,P-Q) << "\t" << a << "\n";
		x=xn;
		intersectionPoints.emplace_back(xn);
	}
	std::cout << M << "\n";
	recalculate();
}

double bf::IntersectionObject::movDist(const bf::vec3d& P, const bf::vec3d& Q, const bf::vec3d &P0, const bf::vec3d &t, double d) {
	double ret=0.0;
	for(int i=0;i<3;i++) {
		ret+=fastPow(P[i]-Q[i],2)+fastPow(glm::dot(P-P0,t)-d,2)*movingAdditionWage;
	}
	return ret;
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
void bf::IntersectionObject::onMergePoints(int, int) {}
void bf::IntersectionObject::onRemoveObject(unsigned int) {//ignore
}
void bf::IntersectionObject::onMoveObject(unsigned int index) {
	if(objectArray.getPtr(index)==obj1 || objectArray.getPtr(index)==obj2) {
		recalculate();
	}
}
void bf::IntersectionObject::recalculate() {
	vertices.clear();
	indices.clear();
	if(intersectionPoints.empty()) return;
	vertices.reserve(2*intersectionPoints.size());
	auto* o=obj2 ? obj2 : obj1;
	for(unsigned i=0;i<intersectionPoints.size();i++) {
		const auto& t=intersectionPoints[i];
		auto p=obj1->parameterFunction(t[0],t[1]);
		vertices.emplace_back(p.x,p.y,p.z);
		indices.emplace_back(i);
		if(i>0)
			indices.emplace_back(i);
	}
	indices.emplace_back(0);
	unsigned ind=vertices.size();
	for(unsigned i=0;i<intersectionPoints.size();i++) {
		const auto& t=intersectionPoints[i];
		auto p=o->parameterFunction(t[2],t[3]);
		vertices.emplace_back(p.x,p.y,p.z);
		indices.emplace_back(i+ind);
		if(i>0)
			indices.emplace_back(i+ind);
	}
	indices.emplace_back(ind);
	VAO==UINT_MAX ? setBuffers() : glUpdateVertices();
}
