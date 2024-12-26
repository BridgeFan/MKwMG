//
// Created by kamil-hp on 28.08.23.
//

#include "IntersectionObject.h"
#include "ConfigState.h"
#include "Curves/BezierCurveInter.h"
#include "Gizmos/MultiCursor.h"
#include "ImGui/ImGuiUtil.h"
#include "ImGui/imgui_include.h"
#include "MullingPathCreator.h"
#include "ObjectArray.h"
#include "Shader/ShaderArray.h"
#include "Surfaces/BezierSurface0.h"
#include "Util.h"
#include <GL/glew.h>
#include <algorithm>
#include <functional>
#include <iostream>
#include <optional>
#include <queue>
#include <random>

std::mt19937 rd(0);
constexpr int TN=1000;

std::pair<bf::Object *, int> bf::IntersectionObject::convertToCurve() {
	if (!obj1) return {nullptr, 0};
	unsigned beginIndex = objectArray.size();
	objectArray.clearSelection();
	//generate points
	for (const auto &p: intersectionPoints) {
		auto pos = obj1->parameterFunction(p.x, p.y);
		if (obj2) {
			pos = (pos + obj2->parameterFunction(p.z, p.w)) * 0.5;
		}
		objectArray.add<bf::Point>(pos);
	}
	auto curve = new bf::BezierCurveInter(objectArray);
	for (unsigned i = beginIndex; i < objectArray.size(); i++) {
		curve->addPoint(i);
	}
	if (isLooped)
		curve->addPoint(beginIndex);
	for (unsigned i = 0; i < beginIndex; i++) {
		if (objectArray.getPtr(i) == this)
			return {curve, i};
	}
	return {curve, 0};
}

bf::IntersectionObject::IntersectionObject(bf::ObjectArray &array) : ObjectArrayListener(array) {
	obj1 = obj2 = nullptr;
	for (unsigned i = 0; i < objectArray.size(); i++) {
		if (objectArray.isActive(i) && objectArray[i].isIntersectable()) {
			if (!obj1) {
				obj1 = &objectArray[i];
				obj1->indestructibilityIndex++;
			} else if (!obj2) {
				obj2 = &objectArray[i];
				obj2->indestructibilityIndex++;
				break;
			}
		}
	}
	isInitPhase = true;
	if (!obj1) {
		toRemove = true;
	}
}

bf::IntersectionObject::IntersectionObject(bf::ObjectArray &array, bf::Object &o1, bf::Object &o2, const glm::vec3& cursor, float precision, bool isImprovedNeeded):
		ObjectArrayListener(array), obj1(&o1) {
	if(std::addressof(o1)==std::addressof(o2))
		obj2 = nullptr;
	else
		obj2 = &o2;
	isInitPhase = false;
	transform.position = cursor;
	objectArray.isForcedActive = false;
	findIntersection(true, precision, isImprovedNeeded);
}

bf::IntersectionObject::IntersectionObject(bf::ObjectArray &array, bf::Object &o1, bf::Object &o2, float precision):
		ObjectArrayListener(array), obj1(&o1) {
	if (std::addressof(o1) == std::addressof(o2))
		obj2 = nullptr;
	else
		obj2 = &o2;
	isInitPhase = false;
	objectArray.isForcedActive = false;
	findIntersection(false, precision);
}


void bf::IntersectionObject::ObjectGui() {
	bf::imgui::checkChanged("Intersection name", name);
	if (isInitPhase) {
		if (!obj1) {
			ImGui::Text("Selected no intersectable objects");
			if (ImGui::Button("OK")) {
				isInitPhase = false;
				objectArray.isForcedActive = false;
			}
		} else {
			static bool isCursor;
			static float precision = .01f;
			ImGui::Checkbox("Use cursor", &isCursor);
			bf::imgui::checkChanged("Cursor position", transform.position);
			bf::imgui::checkChanged("Precision", precision, .001f, 10.f, .001f, .05f);
			if (ImGui::Button("Confirm")) {
				findIntersection(isCursor, precision);
				isInitPhase = false;
				objectArray.isForcedActive = false;
			}
		}
	} else {
		ImGui::Checkbox("Is shown", &isShown);
		if (ImGui::Button("Convert to curve")) {
			auto curve = convertToCurve();
			if (!curve.first) return;
			auto thisObject = std::move(objectArray.objects[curve.second]).first;
			objectArray.objects[curve.second].first.reset(curve.first);
		}
		static bool isSecondSet = false;
		if (isLooped) {
			/*void *imPoint;
			if (obj2 && isSecondSet) {
				imPoint = reinterpret_cast<void *>(static_cast<intptr_t>(obj2->textureID));
			} else {
				imPoint = reinterpret_cast<void *>(static_cast<intptr_t>(obj1->textureID));
			}
			ImGui::Image(imPoint, ImVec2(250, 250));
			if (obj1 && ImGui::Button(obj1->textureMode ? "Unset Object 1 trim" : "Set Object 1 trim")) {
				obj1->textureMode = (obj1->textureMode + 1) % 3;
			}
			if (obj2 && ImGui::Button(obj2->textureMode ? "Unset Object 2 trim" : "Set Object 2 trim")) {
				obj2->textureMode = (obj2->textureMode + 1) % 3;
			}
			std::string a = "Set chosen texture to " + std::to_string(isSecondSet ? 1 : 2);
			if (obj2 && ImGui::Button(a.c_str())) {
				isSecondSet = !isSecondSet;
			}*/
		}
	}
}

void bf::IntersectionObject::mergeFlatIntersections(bf::IntersectionObject &i2) {
	//assumes that o2 will be removed and flat is on obj2
	glm::vec3 P = vertices.back().getPosition();
	glm::vec3 A1 = i2.vertices[0].getPosition();
	glm::vec3 A2 = i2.vertices.back().getPosition();
	if(glm::dot(P-A1,P-A1)>glm::dot(P-A2,P-A2)) {
		std::ranges::reverse(i2.intersectionPoints);
		A1 = i2.vertices.back().getPosition();
		A2 = i2.vertices[0].getPosition();
	}
	if(glm::dot(A1-P,A1-P) > 0.0100) {
		fillHole(intersectionPoints.back(), i2.intersectionPoints[0]);
	}
	intersectionPoints.insert(intersectionPoints.end(), i2.intersectionPoints.begin(), i2.intersectionPoints.end());
	P = vertices[0].getPosition();
	if(glm::dot(P-A2,P-A2) > 0.0100) {
		fillHole(intersectionPoints.back(), intersectionPoints[0]);
	}
	isLooped = true;
	//TODO - uncomment
	recalculate(/*true*/);
	setBuffers();
}

bf::IntersectionObject::~IntersectionObject() {
	if(obj1 && obj1->indestructibilityIndex>0) {
		obj1->indestructibilityIndex--;
		//obj1->textureMode=0;
	}
	if(obj2 && obj2->indestructibilityIndex>0) {
		obj2->indestructibilityIndex--;
		//obj2->textureMode=0;
	}
}

void bf::IntersectionObject::draw(const bf::ShaderArray& shaderArray) const {
	//Solid::draw(shaderArray);
	if(shaderArray.getActiveIndex()!=bf::ShaderType::BasicShader || indices.empty() || !isShown) return;
	auto color = shaderArray.getColor();
	shaderArray.setColor(255,255,0);
	glBindVertexArray(VAO);
	shaderArray.getActiveShader().setMat4("model", glm::mat4(1.f));
	shaderArray.getActiveShader().setFloat("pointSize", 2.f*configState->pointRadius);
	shaderArray.setColor(0,255,0);
	glDrawArrays(GL_POINTS, 0, 1);
	glDrawArrays(GL_POINTS, vertices.size()/2, 1);
	shaderArray.setColor(255,0,0);
	glDrawArrays(GL_POINTS, vertices.size()/2-1, 1);
	glDrawArrays(GL_POINTS, vertices.size()-1, 1);
	shaderArray.setColor(255,255,0);
	glDrawArrays(GL_POINTS, 1, vertices.size()/2-2);
	glDrawArrays(GL_POINTS, vertices.size()/2+1, vertices.size()/2-2);
	shaderArray.setColor(255,255,0);
	glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT,   // type
				   reinterpret_cast<void*>(0));         // element array buffer offset
	shaderArray.setColor(color);
}

bf::ShaderType bf::IntersectionObject::getShaderType() const {
	return bf::ShaderType::MultipleShaders;
}

void clampParams(bf::vec2d& x, const bf::Object& obj, const bf::vec2d& old) {
	for(int i=0;i<2;i++) {
		if(!obj.parameterWrapping()[i]) {
			if(std::abs(old[i]-x[i])<1e-10) //no old point set
				x[i]=std::clamp(x[i],obj.getParameterMin()[i],obj.getParameterMax()[i]);
			else {
				auto m = obj.getParameterMin()[i];
				auto M = obj.getParameterMax()[i];
				if(x[i]<m) {
					x=lerp(x,old,(x[i]-m)/(x[i]-old[i]));
				}
				else if(x[i]>M) {
					x=lerp(x,old,(M-x[i])/(old[i]-x[i]));
				}
			}
		}
		else {
			x[i]=std::fmod(x[i],obj.getParameterMax()[i]);
		}
		if(x[i]<0.0) x[i]+=obj.getParameterMax()[i];
	}
}

void clampParams(bf::vec4d& x, const bf::Object& o1, const bf::Object& o2, const bf::vec4d& old) {
	bf::vec2d x1={x.x,x.y},x2={x.z,x.w};
	bf::vec2d old1={old.x,old.y},old2={old.z,old.w};
	clampParams(x1,o1, old1);
	clampParams(x2,o2, old2);
	x={x1.x, x1.y, x2.x,x2.y};
}

//for numerical methods
constexpr double epsDist=1e-10;
constexpr double eps=1e-6;
constexpr double epsBig=1e-4;

bf::vec2d findBeginningPoint(const bf::Object& obj, const glm::vec3& pos, bool isImproved=false, const bf::Object* other=nullptr, const bf::vec2d& t={}) {
	double minDist=std::numeric_limits<double>::max();;
	static std::uniform_real_distribution<> dis(0.,1.);
	//init
	bf::vec2d x, xn;
	for(int i=0;i<500+2500*(other!=nullptr)+10000*isImproved;i++) {
		bf::vec2d tp;
		tp.x=lerp(obj.getParameterMin()[0],obj.getParameterMax()[0],dis(rd));
		tp.y=lerp(obj.getParameterMin()[1],obj.getParameterMax()[1],dis(rd));
		double dist=bf::IntersectionObject::distance(obj,pos,{tp.x,tp.y});
		if(other!=nullptr) { //to ensure that these points are far from each other if looking for self-intersection
			dist+=1.0/(glm::dot(tp-t,tp-t));
		}
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
		clampParams(xn, obj, x);
		while(bf::IntersectionObject::distance(obj,pos,xn)>bf::IntersectionObject::distance(obj,pos,x) && a>eps) { //too big step
			a*=.25;
			xn=x-df*a;
			clampParams(xn, obj, x);
		}
		N++;
	}
	x=xn;
	std::cout << N /*<< " " << x.x << " " << x.y*/ << "\n";
	//std::cout << a << " " << glm::dot(df,df) << " " << glm::dot(xn-x,xn-x) << "\n";
	return x;
}

constexpr double movingAdditionWage = 0.1;

bool isModAlmostOne(double a, double mod, double epsAO=1e-5) {
	double b=fmod(a, mod);
	if(b<0.0) b+=a;
	return b<epsAO || b-a>-epsAO;
}

bool isOutOfRange(const bf::Object& obj, const bf::vec2d& v) {
	for(int i=0;i<2;i++) {
		if (!obj.parameterWrapping()[i]) {
			if (v[i] > obj.getParameterMax()[i] || v[i] < obj.getParameterMin()[i])
				return true;
		}
	}
	return false;
}

bool isOutOfRange(const bf::Object& o1, const bf::Object& o2, const bf::vec4d& v) {
	bf::vec2d v1 = {v.x,v.y};
	bf::vec2d v2 = {v.z,v.w};
	return isOutOfRange(o1,v1) || isOutOfRange(o2, v2);
}

bool isAbsFmod(double a, double mod, double epsil) {
	double c=std::fmod(a,mod);
	if(c<0.0)
		c+=mod;
	return c<epsil || mod-c<epsil;
}

std::pair<std::vector<bf::vec4d>, bool> moveAlong(bf::Object* obj1, bf::Object* o, const bf::vec4d& x0, double precision, double direction=1.0, bool isImprovedNeeded=false) {
	auto xn=x0, x=x0;
	constexpr double eps2=1e-5;
	constexpr double eps2dist=1e-7;
	int M, N;
	double a;
	bf::vec4d oldX, df;
	std::vector<bf::vec4d> intersectionPoints;
	bool isLooped=false;
	precision*=direction;
	for(M=0;M<2000;M++) {
	//TODO - add "jumping over" singularities
		using I=bf::IntersectionObject;
		//simple gradient is enough here
		auto y=x-x0;
		if(M>=10 && glm::dot(y,y)<precision*precision) {
			isLooped=true;
			break;
		}
		N = 0;
		a=std::abs(precision)*5.0;
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
		isLooped=false;
		while (N < 10000 && a > eps2 && I::movDist(P, Q, P0, t, precision) > eps2dist && glm::dot(df, df) > epsDist /*&& glm::dot(xn - x, xn - x) > epsDist*/) {
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
			clampParams(xn, *obj1, *o, x);
			Pn = obj1->parameterFunction(xn[0], xn[1]);
			auto Qn = o->parameterFunction(xn[2], xn[3]);
			while ((I::movDist(Pn, Qn, P0, t, precision) > I::movDist(P, Q, P0, t, precision)|| isOutOfRange(*obj1,*o,xn)) && a > eps2) {//too big step
				a *= 0.25;
				xn = x - df * a;
				clampParams(xn, *obj1, *o, x);
				Pn = obj1->parameterFunction(xn[0], xn[1]);
				Qn = o->parameterFunction(xn[2], xn[3]);
			}
			N++;
		}
		P = obj1->parameterFunction(xn[0], xn[1]);
		Q = o->parameterFunction(xn[2], xn[3]);
		//std::cout << N << "   \t" << glm::distance(P,P0) << "\t" << glm::dot(P-Q,P-Q) << "\t" << a << "\n";
		if(N>=10000 || std::abs(glm::distance(P,P0)/std::abs(precision)-1.0)>0.5) {
			bool shouldContinue = false;
			/*std::cout << "Stopped\n";
			std::cout << oldX << " " << xn << "\n";
			std::cout << glm::distance(P,P0) << "\n";
			std::cout << N << "\n";
			auto xmin = glm::min(oldX, xn);
			auto xmax = glm::max(oldX, xn);
			for (int i=0;i<4;i++) {
				const std::vector<double> *singularVector;
				if (i==0) singularVector = &obj1->singularU();
				if (i==1) singularVector = &obj1->singularV();
				if (i==2) singularVector = &o->singularU();
				if (i==3) singularVector = &o->singularV();
				if (singularVector)
				for (auto u: *singularVector) {
					if (xmin[i]<u && xmax[i]>u) {
						shouldContinue = true;
						break;
					}
				}
				if (shouldContinue)
					break;
			}*/
			if (!shouldContinue) {
				isLooped=false;
				break;
			}
		}
		x=xn;
		intersectionPoints.emplace_back(xn);
	}
	std::cout << M << "\n";
	return {intersectionPoints, isLooped};
}

void bf::IntersectionObject::findIntersection(bool isCursor, double precision, bool isImprovedNeeded) {
	auto* o = obj2 ? obj2 : obj1;
	auto pos2 = transform.position;
	//no point chosen
	bf::vec4d x0;
	//begin data to algorithm
	if(isCursor) { //cursor begin
		//std::cout << pos2.x << " " << pos2.y << " " << pos2.z << "\n";
		//find beginning point - u,v
		auto t1 = findBeginningPoint(*obj1, pos2, isImprovedNeeded);
		bf::vec2d t2;
		if(!obj2)
			t2 = findBeginningPoint(*o, pos2, isImprovedNeeded, obj1, t1);
		else
			t2 = findBeginningPoint(*o, pos2, isImprovedNeeded);
		x0={t1.x,t1.y,t2.x,t2.y};
		//std::cout << bf::IntersectionObject::distance(*obj1, *o, x0) << "\n";
	}
	else {
		double minDist=std::numeric_limits<double>::max();;
		static std::uniform_real_distribution<> dis(0.,1.);
		for(int i=0;i<50+2500*(!obj2);i++) {
			bf::vec4d tp;
			tp.x=lerp(obj1->getParameterMin()[0],obj1->getParameterMax()[0],dis(rd));
			tp.y=lerp(obj1->getParameterMin()[1],obj1->getParameterMax()[1],dis(rd));
			tp.z=lerp(o->getParameterMin()[0],o->getParameterMax()[0],dis(rd));
			tp.w=lerp(o->getParameterMin()[1],o->getParameterMax()[1],dis(rd));
			double dist= distance(*obj1,*o,tp);
			if(!obj2) { //to ensure that these points are far from each other if looking for self-intersection
				dist+=1.0/(fastPow(tp.x-tp.z,2)+fastPow(tp.y-tp.w,2));
			}
			if(dist<minDist) {
				minDist=dist;
				x0=tp;
			}
		}
	}
	//2 - minimizing distance
	clampParams(x0, *obj1, *o, x0);
	bf::vec4d x=-x0;
	bf::vec4d xn=x0;
	std::cout << xn.x << " " << xn.y << " " << xn.z << " " << xn.w << "\nLooking for first point:\n";
	bf::vec4d df{100000.};
	int N=0;
	double a=0.1;
	while(N<10000 && a>eps && distance(*obj1,*o,xn)>epsDist && glm::dot(df,df)>epsDist && glm::dot(xn-x,xn-x)>eps*eps) {
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
		clampParams(xn, *obj1, *o, x);
		while((distance(*obj1,*o,xn)>distance(*obj1,*o,x) || isOutOfRange(*obj1,*o,xn)) && a>eps) { //too big step
			a*=0.25;
			xn=x-df*a;
			clampParams(xn, *obj1, *o, x);
		}
		N++;
	}
	std::cout << N /*<< " " << a << " " << glm::dot(df,df) << " " << glm::dot(xn-x,xn-x)*/ << "\n";
	x=xn;
	//std::cout << N << " " << distance(*obj1, *o, x) << "\n";
	if(distance(*obj1,*o,x)>epsBig) {
		std::cout << "No intersection found\n";
		//TODO - remove
		auto v=bf::toFloatVec<3>(obj1->parameterFunction(x[0],x[1]));
		auto w=bf::toFloatVec<3>(o->parameterFunction(x[2],x[3]));
		objectArray.add<bf::Point>(v);
		objectArray.add<bf::Point>(w);
		toRemove=true;
		return;
	}
	std::cout << "March\n";
	intersectionPoints.emplace_back(x);
	//move along (both sides if no loop)
	setBuffers();
	auto [pts, loop] = moveAlong(obj1,o,x,precision, 1.0, isImprovedNeeded);
	if(!loop) {
		auto [pts2, loop2] = moveAlong(obj1,o,x,precision,-1.0, isImprovedNeeded);
		//try the other side
		std::vector<bf::vec4d> revPts2(pts2.rbegin(),pts2.rend());
		revPts2.emplace_back(x);
		revPts2.insert(revPts2.end(),
								  std::make_move_iterator(pts.begin()),
								  std::make_move_iterator(pts.end()));
		intersectionPoints=std::move(revPts2);

	}
	else {//ready to insert
		intersectionPoints.insert(intersectionPoints.end(),
								  std::make_move_iterator(pts.begin()),
								  std::make_move_iterator(pts.end()));
	}
	isLooped=loop;
	//TODO: uncomment
	recalculate(/*true*/);
	setBuffers();
}

double bf::IntersectionObject::movDist(const bf::vec3d &P, const bf::vec3d &Q, const bf::vec3d &P0, const bf::vec3d &t, double d) {
	double ret = 0.0;
	for (int i = 0; i < 3; i++) {
		ret += fastPow(P[i] - Q[i], 2) + fastPow(glm::dot(P - P0, t) - d, 2) * movingAdditionWage;
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
	if (objectArray.getPtr(index) == obj1 || objectArray.getPtr(index) == obj2) {
		recalculate();
	}
}

int drawToTexture(const std::vector<bf::vec4d>& v, bool isSecond, const bf::vec2d& maks, bool wrapX, bool wrapY) {
	std::vector<uint8_t> val(TN*TN, 0);
	glm::vec2 p(0.0f,0.0f);
	//draw pixels
	for(unsigned i=0;i<v.size();i++) {
		unsigned j=(i+1)%v.size();
		int x1 = static_cast<int>((isSecond ? v[i].z : v[i].x)/maks.x*TN);
		int y1 = static_cast<int>((isSecond ? v[i].w : v[i].y)/maks.y*TN);
		int x2 = static_cast<int>((isSecond ? v[j].z : v[j].x)/maks.x*TN);
		int y2 = static_cast<int>((isSecond ? v[j].w : v[j].y)/maks.y*TN);
		p.x+=x1;
		p.y+=y1;
		BresenhamLine(val, TN, x1, y1, x2, y2, 127u);
	}
	p/=v.size();
	int px=static_cast<int>(p.x);
	int py=static_cast<int>(p.y);
	int i=0;
	while(val[px+TN*py]==127u) {
		px=(px+1)%TN;
		i++;
		if(i==TN) {
			i=0;
			py=(py+1)%TN;
		}
	}
	//flood fill
	floodFill(val, TN, static_cast<int>(p.x), static_cast<int>(p.y), wrapX, wrapY, 255u);
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load and generate the texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, TN, TN, 0, GL_RED, GL_UNSIGNED_BYTE, val.data());
	glGenerateMipmap(GL_TEXTURE_2D);
	return texture;
}

void bf::IntersectionObject::recalculate(bool isTextureToSet) {
	vertices.clear();
	indices.clear();
	if (intersectionPoints.empty()) return;
	vertices.reserve(2 * intersectionPoints.size());
	auto *o = obj2 ? obj2 : obj1;
	for (unsigned i = 0; i < intersectionPoints.size(); i++) {
		const auto &t = intersectionPoints[i];
		auto p = obj1->parameterFunction(t[0], t[1]);
		vertices.emplace_back(p.x, p.y, p.z);
		indices.emplace_back(i);
		if (i > 0 && (isLooped || i < intersectionPoints.size() - 1))
			indices.emplace_back(i);
	}
	if (isLooped)
		indices.emplace_back(0);
	unsigned ind = vertices.size();
	for (unsigned i = 0; i < intersectionPoints.size(); i++) {
		const auto &t = intersectionPoints[i];
		auto p = o->parameterFunction(t[2], t[3]);
		vertices.emplace_back(p.x, p.y, p.z);
		indices.emplace_back(i + ind);
		if (i > 0 && (isLooped || i < intersectionPoints.size() - 1))
			indices.emplace_back(i + ind);
	}
	if (isLooped)
		indices.emplace_back(ind);
	VAO == UINT_MAX ? setBuffers() : glUpdateVertices();
	if (isTextureToSet && isLooped) {
		if (obj1)
			obj1->textureID = drawToTexture(intersectionPoints, false, obj1->getParameterMax(), obj1->parameterWrappingU(), obj1->parameterWrappingV());
		if (obj2)
			obj2->textureID = drawToTexture(intersectionPoints, true, obj2->getParameterMax(), obj2->parameterWrappingU(), obj2->parameterWrappingV());
	}
}
void bf::IntersectionObject::fillHole(const bf::vec4d &a, const bf::vec4d &b) {
	//there are no toruses to fill
	auto mullingObject = dynamic_cast<MullingPathCreator*>(obj2);
	//XYZW - o1.u, o1.v, o2.u, o2.v
	constexpr int STEPS_PER_UNIT = 3;
	bf::vec4d act=a;
	double steps;
	//TODO
	if(obj1->parameterWrappingU()) {
		steps = STEPS_PER_UNIT*std::abs(a.x-b.x);
		//U const case
		if((obj1->parameterGradientV(a.x,a.y).z > 0.0) ^ (a.x>b.x)) {
			act.x -= obj1->getParameterMax().y;
		}
	}
	else {
		steps = STEPS_PER_UNIT*std::abs(a.y-b.y);
		//V const case
		if((obj1->parameterGradientU(a.x,a.y).z > 0.0) ^ (a.y>b.y)) {
			act.y -= obj1->getParameterMax().x;
		}
	}
	steps = std::ceil(steps);
	std::cout << a << "\n";
	for(int i=1;i<steps-0.05;i++) {
		act.x=lerp(a.x,b.x,static_cast<float>(i)/(steps-1.0));
		act.y=lerp(a.y,b.y,static_cast<float>(i)/(steps-1.0));
		auto pos = obj1->parameterFunction(act.x, act.y);
		auto p=mullingObject->toSurfaceSpace({pos.x, pos.y});
		act.z = p.x;
		act.a = p.y;
		intersectionPoints.push_back(act);
		std::cout << act << "\n";
	}
	std::cout << b << "\n";
}
