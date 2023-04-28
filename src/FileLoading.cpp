//
// Created by kamil-hp on 22.04.23.
//
#include <iostream>
#include "FileLoading.h"
#include "Serializer.h"
#include "Object/ObjectArray.h"
#include "Solids/Point.h"
#include "Solids/Torus.h"
#include "Curves/BezierCurve0.h"
#include "Curves/BezierCurve2.h"
#include "Curves/BezierCurveInter.h"

MG1::SceneSerializer serializer;

glm::vec3 toVector(const MG1::Float3& f) {
	return {f.x,f.y,f.z};
}
MG1::Float3 toFloat3(const glm::vec3& f) {
	return {f.x,f.y,f.z};
}

bool bf::loadFromFile(const std::string &path, bf::ObjectArray &objectArray) {
	try {
		serializer.LoadScene(path);
	}
	catch(...) {
		return false;
	}
	auto& scene = MG1::Scene::Get();
	objectArray.removeAll();
	const unsigned totalLength = scene.points.size()+scene.tori.size()+scene.bezierC0.size()+scene.bezierC2.size()+
								 scene.interpolatedC2.size()+scene.surfacesC0.size()+scene.surfacesC2.size();
	objectArray.objects.resize(totalLength);
	for(auto& o: objectArray.objects)
		o.second = false;
	//TODO - surfacesC0, surfacesC2
	for(const auto& p: scene.points) {
		int id = p.GetId()-1;
		bf::Transform t(toVector(p.position));
		objectArray.objects[id].first=std::make_unique<bf::Point>(t,p.name);
	}
	for(const auto& t: scene.tori) {
		int id = t.GetId()-1;
		bf::Transform transform(toVector(t.position),
			toVector(t.rotation),toVector(t.scale));
		objectArray.objects[id].first=std::make_unique<bf::Torus>(transform,t.name,
			t.largeRadius,t.smallRadius,t.samples.x,t.samples.y);
	}
	for(const auto& b0: scene.bezierC0) {
		int id = b0.GetId()-1;
		auto obj = std::make_unique<bf::BezierCurve0>(objectArray, b0.name);
		for(const auto& a: b0.controlPoints) {
			obj->addPoint(a.GetId());
		}
		objectArray.objects[id].first=std::move(obj);
	}
	for(const auto& b2: scene.bezierC2) {
		int id = b2.GetId()-1;
		auto obj = std::make_unique<bf::BezierCurve2>(objectArray,b2.name);
		for(const auto& a: b2.controlPoints) {
			obj->addPoint(a.GetId());
		}
		objectArray.objects[id].first=std::move(obj);
	}
	for(const auto& bi: scene.interpolatedC2) {
		int id = bi.GetId()-1;
		auto obj = std::make_unique<bf::BezierCurveInter>(objectArray,bi.name);
		for(const auto& a: bi.controlPoints) {
			obj->addPoint(a.GetId());
		}
		objectArray.objects[id].first=std::move(obj);
	}
	return true;
}

bool bf::saveToFile(const std::string &path, const bf::ObjectArray &objectArray) {
	auto& scene = MG1::Scene::Get();
	scene.Clear();
	//TODO - save surfacesC0, surfacesC2
	for(unsigned i=0;i<objectArray.size();i++) {
		const auto& o = objectArray[i];
		if(typeid(*&o)==typeid(bf::Point)) {
			const auto p = dynamic_cast<const bf::Point*>(&o);
			MG1::Point point;
			point.position = toFloat3(p->getPosition());
			point.name = p->name;
			point.SetId(i);
			scene.points.emplace_back(std::move(point));
		}
		else if(typeid(*&o)==typeid(bf::Torus)) {
			const auto t = dynamic_cast<const bf::Torus*>(&o);
			MG1::Torus torus;
			torus.position = toFloat3(t->getPosition());
			torus.rotation = toFloat3(t->getRotation());
			torus.scale = toFloat3(t->getScale());
			torus.name = t->name;
			torus.largeRadius = t->bigRadius;
			torus.smallRadius = t->smallRadius;
			torus.samples = {static_cast<uint32_t>(t->bigFragments), static_cast<uint32_t>(t->smallFragments)};
			torus.SetId(i);
			scene.tori.emplace_back(std::move(torus));
		}
		else if(typeid(*&o)==typeid(bf::BezierCurve0)) {
			const auto b = dynamic_cast<const bf::BezierCurve0*>(&o);
			MG1::BezierC0 bezier;
			bezier.name = b->name;
			for(const auto& p: b->usedVectors()) {
				bezier.controlPoints.emplace_back(p);
			}
			bezier.SetId(i);
			scene.bezierC0.emplace_back(std::move(bezier));
		}
		else if(typeid(*&o)==typeid(bf::BezierCurve2)) {
			const auto b = dynamic_cast<const bf::BezierCurve2*>(&o);
			MG1::BezierC2 bezier;
			bezier.name = b->name;
			for(const auto& p: b->usedVectors()) {
				bezier.controlPoints.emplace_back(p);
			}
			bezier.SetId(i);
			scene.bezierC2.emplace_back(std::move(bezier));
		}
		else if(typeid(*&o)==typeid(bf::BezierCurveInter)) {
			const auto b = dynamic_cast<const bf::BezierCurveInter*>(&o);
			MG1::InterpolatedC2 bezier;
			bezier.name = b->name;
			for(const auto& p: b->usedVectors()) {
				bezier.controlPoints.emplace_back(p);
			}
			bezier.SetId(i);
			scene.interpolatedC2.emplace_back(std::move(bezier));
		}
		else {
			std::cout << "Not implemented type\n";
		}
	}
	try {
		serializer.SaveScene(path);
	}
	catch(...) {
		return false;
	}
	return true;
}

