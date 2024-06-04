//
// Created by kamil-hp on 28.08.23.
//

#include "IntersectionObject.h"
#include "Curves/BezierCurveInter.h"
#include "ObjectArray.h"
#include "ImGui/imgui_include.h"
#include "ImGui/ImGuiUtil.h"
#include "Shader/ShaderArray.h"


bf::ObjectArray* bf::IntersectionObject::objectArray = nullptr;
std::pair<bf::Object*, int> bf::IntersectionObject::convertToCurve() {
	if(!obj1) return {nullptr, 0};
	unsigned beginIndex = objectArray->size();
	objectArray->clearSelection();
	//generate points
	for(const auto& p: intersectionPoints) {
		auto pos = obj1->parameterFunction(p.x,p.y);
		if(obj2) {
			pos = (pos + obj2->parameterFunction(p.z,p.w)) * .5f;
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
	for(int i=0;i<beginIndex;i++) {
		if(objectArray->getPtr(i)==this)
			return {curve, i};
	}
	return {curve, 0};
}
bf::IntersectionObject::IntersectionObject() {
	if(!objectArray)
		return;
	for(int i=0;i<objectArray->size();i++) {
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
	if(obj1)
		objectArray->isForcedActive=true;
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
				//TODO - calculate intersection using Newton method
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
	if(obj1->indestructibilityIndex>0)
		obj1->indestructibilityIndex--;
	if(obj2 && obj2->indestructibilityIndex>0)
		obj2->indestructibilityIndex--;
}
void bf::IntersectionObject::onMergePoints(int p1, int p2) {}
void bf::IntersectionObject::draw(const bf::ShaderArray &shader) const {
	//TODO
}
bf::ShaderType bf::IntersectionObject::getShaderType() const {
	return bf::ShaderType::MultipleShaders;
}
void bf::IntersectionObject::updatePoints() {
	//TODO
	if(!obj1) return;
	std::vector<bf::Vertex> vertices;
	for(const auto& p: intersectionPoints) {
		/*if(obj2)
			vertices.*/
	}

}
