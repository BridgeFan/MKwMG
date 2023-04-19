//
// Created by kamil-hp on 27.03.23.
//

#include <algorithm>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "BezierCurve.h"
#include "Object/ObjectArray.h"
#include "Solids/Point.h"
#include "ImGuiUtil.h"
#include "imgui-master/imgui.h"
#include "Shader.h"
#include "Scene.h"

int bf::BezierCurve::_index = 1;

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

void bf::BezierCurve::bezierOnAdd() {
	bezier.points.push_back(objectArray[pointIndices.back()].getPosition());
	bezier.recalculate(true);
}

void bf::BezierCurve::bezierOnRemove(unsigned int index) {
	for(unsigned i=index+1;i<bezier.points.size();i++) {
		std::swap(bezier.points[i-1],bezier.points[i]);
	}
	bezier.points.pop_back();
	bezier.recalculate(true);
}

void bf::BezierCurve::bezierOnSwap(unsigned int index1, unsigned int index2) {
	std::swap(bezier.points[index1],bezier.points[index2]);
	bezier.recalculate(false);
}

void bf::BezierCurve::bezierOnMove(unsigned int index) {
	bezier.points[index]=objectArray[pointIndices[index]].getPosition();
	bezier.recalculate(false);
}

bf::BezierCurve::BezierCurve(bf::ObjectArray &array) : BezierCommon(array) {
	name = "Bézier curve0 " + std::to_string(_index);
	_index++;
}
