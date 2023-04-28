//
// Created by kamil-hp on 27.03.23.
//

#include <algorithm>
#include "BezierCurve2.h"
#include "Object/ObjectArray.h"
#include "src/ImGui/ImGuiUtil.h"
#include "Util.h"
#include "Scene.h"
#include "ConfigState.h"
#include "Event.h"

int bf::BezierCurve2::_index = 1;

template<typename T>
std::size_t binSearch(const std::vector<T>& sorted, std::size_t value, std::size_t begin, std::size_t end) {
	if(begin==end)
		return begin;
	std::size_t index = (begin+end)/2u;
	if(sorted[index]>value)
		return binSearch(sorted, value, begin, index-1);
	else if(sorted[index]==value)
		return index;
	else
		return binSearch(sorted, value, index+1, end);
}
template<typename T>
std::size_t binSearch(const std::vector<T>& sorted, std::size_t value) {
	return binSearch(sorted,value,sorted.begin(),sorted.end()-1);
}

void bf::BezierCurve2::bezierOnAdd() {
	activeBezierIndex=-1;
    recalculate();
}

void bf::BezierCurve2::bezierOnRemove(unsigned int) {
	activeBezierIndex=-1;
	recalculate();
}

void bf::BezierCurve2::bezierOnSwap(unsigned int, unsigned int) {
	activeBezierIndex=-1;
	recalculate();
}

void bf::BezierCurve2::bezierOnMove(unsigned int index) {
	if(pointIndices.size()<=3 || index>=pointIndices.size())
		return;
	recalculate();
}

bf::BezierCurve2::BezierCurve2(bf::ObjectArray &array) : BezierCommon(array) {
	name = "Bézier curve2 " + std::to_string(_index);
	_index++;
    isTmpLineDrawn=true;
	isTmpPointDrawn=true;
}
bf::BezierCurve2::BezierCurve2(bf::ObjectArray &array, const std::string& bName) : BezierCommon(array) {
	name = bName;
	isTmpLineDrawn=true;
	isTmpPointDrawn=true;
}

void bf::BezierCurve2::recalculate() {
	//update all points
	std::vector<glm::vec3> points;
	for(const auto& i: pointIndices)
		if(objectArray.isCorrect(i))
			points.push_back(objectArray[i].getPosition());
	auto newPoints = bf::bezier2ToBezier0(points);
	bool wasResized = newPoints.size()!=points.size();
	bezier.points = std::move(newPoints);
	bezier.recalculate(wasResized);
}

bool bf::BezierCurve2::onMouseButtonPressed(bf::event::MouseButton button, bf::event::ModifierKeyBit) {
	if(!scene || !configState || button!=bf::event::MouseButton::Left)
		return false;
	const float& mouseXF = configState->mouseX;
    const float& mouseYF = configState->mouseY;
	constexpr float sqrDist = 64.f;
	int selectionIndex = -1;
	float actualZ = 9.999f;
	for(unsigned i=0u;i<bezier.points.size();i++) {
		auto screenPos = bf::toScreenPos(configState->screenWidth, configState->screenHeight, bezier.points[i], scene->getView(), scene->getProjection());
		if(screenPos==bf::outOfWindow)
			continue;
		float d = (screenPos.x-mouseXF)*(screenPos.x-mouseXF)+(screenPos.y-mouseYF)*(screenPos.y-mouseYF);
		if(d<=sqrDist && actualZ>screenPos.z) {
			selectionIndex=static_cast<int>(i);
			actualZ=screenPos.z;
		}
	}
	activeBezierIndex = selectionIndex;
	return activeBezierIndex>=0;
}

bool bf::BezierCurve2::onMouseButtonReleased(bf::event::MouseButton button, bf::event::ModifierKeyBit) {
	if(button==bf::event::MouseButton::Left) {
		activeBezierIndex = -1;
		return true;
	}
	return false;
}

void bf::BezierCurve2::onMouseMove(const glm::vec2&, const glm::vec2 &newPos) {
	if(activeBezierIndex<0)
		return;
	float z = bf::toScreenPos(configState->screenWidth, configState->screenHeight,bezier.points[activeBezierIndex],scene->getView(),scene->getProjection()).z;
	glm::vec3 newPosZ = {newPos.x,newPos.y,z};
	auto newWorldPos = bf::toGlobalPos(configState->screenWidth, configState->screenHeight,newPosZ,scene->getInverseView(),scene->getInverseProjection());
	auto difVector = newWorldPos - bezier.points[activeBezierIndex];
	int oaIndex = activeBezierIndex/3+1;
	switch(activeBezierIndex%3) {
		case 0:
			objectArray[pointIndices[oaIndex]].setPosition(getPoint(oaIndex)+difVector);
			break;
		case 1:
			objectArray[pointIndices[oaIndex]].setPosition(getPoint(oaIndex)+2.f*difVector);
			objectArray[pointIndices[oaIndex+1]].setPosition(getPoint(oaIndex+1)-difVector);
			break;
		case 2:
			objectArray[pointIndices[oaIndex]].setPosition(getPoint(oaIndex)-difVector);
			objectArray[pointIndices[oaIndex+1]].setPosition(getPoint(oaIndex+1)+2.f*difVector);
			break;
		default:
			break;
	}
	recalculate();
}
