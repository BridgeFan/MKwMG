//
// Created by kamil-hp on 27.03.23.
//

#include <algorithm>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "BezierCurveInter.h"
#include "Object/ObjectArray.h"
#include "Solids/Point.h"
#include "ImGuiUtil.h"
#include "imgui-master/imgui.h"
#include "Shader.h"
#include "Scene.h"
#include "Util.h"

int bf::BezierCurveInter::_index = 1;

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

void bf::BezierCurveInter::bezierOnAdd() {
	recalculate(true);
}

void bf::BezierCurveInter::bezierOnRemove(unsigned int) {
	recalculate(true);
}

void bf::BezierCurveInter::bezierOnSwap(unsigned int, unsigned int) {
	recalculate(false);
}

void bf::BezierCurveInter::bezierOnMove(unsigned int) {
	recalculate(false);
}

bf::BezierCurveInter::BezierCurveInter(bf::ObjectArray &array) : BezierCommon(array) {
	isLineDrawn = false;
	name = "Bézier curveI " + std::to_string(_index);
	_index++;
}

void bf::BezierCurveInter::recalculate(bool wasSizeChanged) {
	int n=pointIndices.size();
    if(n<=1) {
        positions.clear();
        bezier.points = bf::bezier2ToBezier0(positions);
        bezier.recalculate(wasSizeChanged);
        return;
    }
	const std::vector<float> a(n-1,1.f);
	const std::vector<float> b(n,4.f);
	const std::vector<float> c(n-1,1.f);
	std::vector<glm::vec3> d(n);
    d[0]=getPoint(0)-getPoint(1);
    d[n-1]=getPoint(n-1)-getPoint(n-2);
	for(int i=1;i<n-1;i++) {
        d[i]=2.f*getPoint(i)-getPoint(i+1)-getPoint(i-1);
	}
	auto newPoints = tridiagonalMatrixAlgorithm(a,b,c,d);
    positions.resize(newPoints.size()+2);
    for(int i=0;i<n;i++) {
        positions[i+1]=getPoint(i)+newPoints[i];
    }
    //0th point
    glm::vec3 t1 = lerp(positions[1],positions[2],1.f/3.f);
    t1 = lerp(getPoint(0),t1,-1.f);
    positions[0]=lerp(positions[1],t1,3.f);
    //last point
    t1 = lerp(positions[n],positions[n-1],1.f/3.f);
    t1 = lerp(getPoint(n-1),t1,-1.f);
    positions[n+1]=lerp(positions[n],t1,3.f);
    //recalculate
    bezier.points = bf::bezier2ToBezier0(positions);
	bezier.recalculate(wasSizeChanged);
	//update GPU data
	if(!wasSizeChanged) {
		glNamedBufferSubData(lVBO, 0, positions.size() * sizeof(glm::vec3), positions.data());
	}
	else {
		if(lVAO<UINT_MAX)
			glDeleteVertexArrays(1, &lVAO);
		if(lVBO<UINT_MAX)
			glDeleteBuffers(1, &lVBO);
		if(lIBO<UINT_MAX)
			glDeleteBuffers(1, &lIBO);
		//set buffers
		if(positions.empty()) {
			return;
		}
		std::vector<unsigned> tmpIndices;
		tmpIndices.reserve(positions.size()*2-2);
		for(int i=0;i<positions.size();i++) {
			if(i>0 && i<static_cast<int>(positions.size())-1)
				tmpIndices.emplace_back(i);
			tmpIndices.emplace_back(i);
		}

		glGenVertexArrays(1, &lVAO);
		glGenBuffers(1, &lVBO);

		glBindVertexArray(lVAO);

		glBindBuffer(GL_ARRAY_BUFFER, lVBO);
		glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3), positions.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(0));
		glEnableVertexAttribArray(0);

		glGenBuffers(1, &lIBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lIBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, tmpIndices.size() * sizeof(unsigned), &tmpIndices[0], GL_STATIC_DRAW);
	}
}

void bf::BezierCurveInter::draw(const bf::Shader &shader) const {
	if(pointIndices.size()>=2 && isPolygonVisible) {
		shader.setVec3("color", 0.f,0.f,0.f);
		shader.setMat4("model", glm::mat4(1.f));
		glBindVertexArray(lVAO);
		glDrawElements(GL_LINES, positions.size() * 2 - 2, GL_UNSIGNED_INT, 0);
	}
    BezierCommon::draw(shader);
}

bf::BezierCurveInter::~BezierCurveInter() {
	glDeleteVertexArrays(1, &lVAO);
	glDeleteBuffers(1, &lVBO);
	glDeleteBuffers(1, &lIBO);
}
