//
// Created by kamil-hp on 27.03.23.
//

#include <algorithm>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "BezierCurve0.h"
#include "Object/ObjectArray.h"
#include "Solids/Point.h"
#include "src/ImGui/ImGuiUtil.h"
#include "imgui-master/imgui.h"
#include "Shader.h"
#include "Scene.h"

int bf::BezierCurve0::_index = 1;

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

void bf::BezierCurve0::bezierOnAdd() {
	bezier.points.push_back(objectArray[pointIndices.back()].getPosition());
	bezier.recalculate(true);
}

void bf::BezierCurve0::bezierOnRemove(unsigned int index) {
	for(unsigned i=index+1;i<bezier.points.size();i++) {
		std::swap(bezier.points[i-1],bezier.points[i]);
	}
	bezier.points.pop_back();
	bezier.recalculate(true);
}

void bf::BezierCurve0::bezierOnSwap(unsigned int index1, unsigned int index2) {
	std::swap(bezier.points[index1],bezier.points[index2]);
	bezier.recalculate(false);
}

void bf::BezierCurve0::bezierOnMove(unsigned int index) {
	bezier.points[index]=objectArray[pointIndices[index]].getPosition();
	bezier.recalculate(false);
}

bf::BezierCurve0::BezierCurve0(bf::ObjectArray &array) : BezierCommon(array) {
	name = "Bézier curve0 " + std::to_string(_index);
	_index++;
}

bf::BezierCurve0::BezierCurve0(bf::ObjectArray &array, const std::string &bName): BezierCommon(array) {
	name=bName;
}
