//
// Created by kamil-hp on 27.03.23.
//

#include <algorithm>
#include <cstddef>
#include "BezierCurve.h"
#include "../Object/ObjectArray.h"
#include "Point.h"
#include "ImGuiUtil.h"
#include "imgui.h"

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

bool bf::BezierCurve::addPoint(std::size_t index) {
	if(!objectArray.isCorrect(index))
		return false;
	indices.push_back(index);
	recalculate();
	return true;
}

bool bf::BezierCurve::removePoint(std::size_t index) {
	if(!objectArray.isCorrect(index) || indices.empty())
		return false;
	for(std::size_t i=index+1u;i<objectArray.size();i++)
		std::swap(indices[i],indices[i+1u]);
	indices.pop_back();
	activeIndex=0;
	recalculate();
	return true;
}

void bf::BezierCurve::onRemoveObject(std::size_t index) {
	std::size_t dist=0u;
	for(std::size_t i=0u;i<indices.size();i++) {
		if(indices[i]==index)
			dist++;
		if(dist>0)
			std::swap(indices[i],indices[i+dist]);
	}
	if(dist>0) {
		for(std::size_t i=0u;i<dist;i++)
			indices.pop_back();
		recalculate();
	}
}

bf::BezierCurve::BezierCurve(bf::ObjectArray &array, const bf::Camera &camera1, const bf::Settings &settings1):
	Object("Bézier curve0 " + std::to_string(_index)), ObjectArrayListener(array), camera(camera1), settings(settings1) {
		_index++;
	for(std::size_t i=0;i<objectArray.size();i++) {
		if(objectArray.isCorrect(i) && typeid(objectArray[i])==typeid(bf::Point)) {
			indices.push_back(i);
		}
	}
}

void bf::BezierCurve::draw(const bf::Shader &shader) const {
	//TODO
}

void bf::BezierCurve::ObjectGui() {
	//TODO
	bf::imgui::checkChanged("Curve name", name);
	if(ImGui::Button("Delete point"))
		removePoint(activeIndex);
	ImGui::Text("List of points");
	for(std::size_t i=0;i<indices.size();i++) {
		if(!objectArray.isCorrect(indices[i]))
			ImGui::Text("_");
		bool val = (i==activeIndex);
		bool ret = bf::imgui::checkSelectableChanged(objectArray[indices[i]].name.c_str(),val);
		if(ret && val) {
			activeIndex=i;
		}
	}
}

void bf::BezierCurve::recalculate() {
	//TODO
}
