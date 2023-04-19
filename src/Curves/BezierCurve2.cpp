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
    /*if(pointIndices.size()<=3)
        return;
	if(pointIndices.size()==4)
		bezier.points.emplace_back();
	bezier.points.emplace_back();
	bezier.points.emplace_back();
	bezier.points.emplace_back();
	bezierOnMove(bezier.points.size()-1);*/
    /*auto p1 = getPoint(pointIndices.size()-3);
    auto p2 = getPoint(pointIndices.size()-2);
    auto p3 = getPoint(pointIndices.size()-1);
    bf::linear auto np1 = lerp(p1,p2,1.f/3.f);
    bf::linear auto np2 = lerp(p1,p2,2.f/3.f);
    bf::linear auto np4 = lerp(p2,p3,1.f/3.f);
    bf::linear auto np3 = lerp(np2,np4,.5f);
    if(pointIndices.size()==4) {
        auto np_1 = lerp(objectArray[pointIndices[0]].getPosition(),p1,2.f/3.f);
        bezier.points.push_back(lerp(np_1,np1,.5f));
    }
    bezier.points.emplace_back(np1);
    bezier.points.emplace_back(np2);
    bezier.points.emplace_back(np3);*/
    recalculate(true);
}

void bf::BezierCurve2::bezierOnRemove(unsigned int index) {
	recalculate(true);
}

void bf::BezierCurve2::bezierOnSwap(unsigned int index1, unsigned int index2) {
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
