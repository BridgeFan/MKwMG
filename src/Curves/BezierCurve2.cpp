//
// Created by kamil-hp on 27.03.23.
//

#include <algorithm>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "BezierCurve2.h"
#include "Object/ObjectArray.h"
#include "Solids/Point.h"
#include "ImGuiUtil.h"
#include "imgui-master/imgui.h"
#include "Shader.h"
#include "Util.h"
#include "GlfwUtil.h"
#include "Scene.h"

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
    recalculate(true);
}

void bf::BezierCurve2::bezierOnRemove(unsigned int index) {
	activeBezierIndex=-1;
	recalculate(true);
}

void bf::BezierCurve2::bezierOnSwap(unsigned int index1, unsigned int index2) {
	activeBezierIndex=-1;
	recalculate(false);
}

void bf::BezierCurve2::bezierOnMove(unsigned int index) {
	if(pointIndices.size()<=3 || index>=pointIndices.size())
		return;
	recalculate(false);
}

bf::BezierCurve2::BezierCurve2(bf::ObjectArray &array) : BezierCommon(array) {
	name = "Bézier curve2 " + std::to_string(_index);
	_index++;
    isTmpLineDrawn=true;
	isTmpPointDrawn=true;
}

glm::vec3 bf::BezierCurve2::getPoint(int pIndex) const {
	int size = static_cast<int>(pointIndices.size());
	if(pIndex>=size)
		pIndex=pIndex%size;
	if(pIndex<0)
		pIndex=(pIndex+size*(-pIndex/size+1))%size;
	return objectArray[pointIndices[pIndex]].getPosition();
}

void bf::BezierCurve2::recalculate(bool wasResized) {
	//update all points
	if(pointIndices.size()>3) {
		bezier.points.resize(3*pointIndices.size()-8);
		for (int i = 1; i < static_cast<int>(pointIndices.size()) - 2; i++) {
			bezier.points[3*i-2]=lerp(getPoint(i),getPoint(i+1),1.f/3.f);
			bezier.points[3*i-1]=lerp(getPoint(i),getPoint(i+1),2.f/3.f);
			if(i==1u)
				bezier.points[0]=lerp(lerp(getPoint(0),getPoint(1),2.f/3.f),bezier.points[1],.5f);
			else
				bezier.points[3*i-3]=lerp(bezier.points[3*i-4],bezier.points[3*i-2],.5f);
		}
		bezier.points[3*pointIndices.size()-9]=
				lerp(bezier.points[3*pointIndices.size()-10],
					 lerp(getPoint(pointIndices.size()-2),getPoint(pointIndices.size()-1),1.f/3.f),
					 .5f);
	}
	else {
		bezier.points.clear();
	}
	bezier.recalculate(wasResized);
}

bool bf::BezierCurve2::onMouseButtonPressed(int button, int) {
	if(!scene || button!=GLFW_MOUSE_BUTTON_LEFT)
		return false;
	double mouseX, mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);
	auto mouseXF = static_cast<float>(mouseX);
	auto mouseYF = static_cast<float>(mouseY);
	constexpr float sqrDist = 64.f;
	int selectionIndex = -1;
	float actualZ = 9.999f;
	for(unsigned i=0u;i<bezier.points.size();i++) {
		auto screenPos = bf::glfw::toScreenPos(window, bezier.points[i], scene->getView(), scene->getProjection());
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

bool bf::BezierCurve2::onMouseButtonReleased(int button, int) {
	if(button==GLFW_MOUSE_BUTTON_LEFT) {
		activeBezierIndex = -1;
		return true;
	}
	return false;
}

void bf::BezierCurve2::onMouseMove(const glm::vec2 &oldPos, const glm::vec2 &newPos) {
	if(activeBezierIndex<=0)
		return;
	printf("%d\n",activeBezierIndex);
	float z = bf::glfw::toScreenPos(window,bezier.points[activeBezierIndex],scene->getView(),scene->getProjection()).z;
	glm::vec3 newPosZ = {newPos.x,newPos.y,z};
	auto newWorldPos = bf::glfw::toGlobalPos(window,newPosZ,scene->getInverseView(),scene->getInverseProjection());
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
	recalculate(false);
}
