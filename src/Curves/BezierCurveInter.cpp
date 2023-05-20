//
// Created by kamil-hp on 27.03.23.
//

#include <algorithm>
#include "OpenGLUtil.h"
#include <iostream>
#include "BezierCurveInter.h"
#include "Object/ObjectArray.h"
#include "ImGui/ImGuiUtil.h"
#include "ImGui/imgui_include.h"
#include "Shader/Shader.h"
#include "Scene.h"
#include "Util.h"
#include "Shader/ShaderArray.h"

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
bf::BezierCurveInter::BezierCurveInter(bf::ObjectArray &array, const std::string& bName) : BezierCommon(array) {
	isLineDrawn = false;
	name = bName;
}

void bf::BezierCurveInter::recalculate(bool wasSizeChanged) {
	int n=pointIndices.size();
    if(n<=1) {
        positions.clear();
        bezier.points.clear();
        bezier.recalculate(wasSizeChanged);
        return;
    }
    //prepare diagonals for solving
    std::vector<float> a(n-1,.5f);
    std::vector<float> b(n,2.f);
    std::vector<float> c(n-1,.5f);
    std::vector<glm::vec3> d(n);
    d[0]=glm::vec3(.0f);
    d[n-1]=glm::vec3(.0f);
    a[n-2]=c[0]=.0f;
    for(int i=1;i<n-1;i++) {
        float di = glm::length(getPoint(i+1)-getPoint(i));
        float di_1 = glm::length(getPoint(i)-getPoint(i-1));
        glm::vec3 npi = (getPoint(i+1)-getPoint(i))/di;
        glm::vec3 npi_1 = (getPoint(i)-getPoint(i-1))/di_1;
        a[i-1]=di_1/(di+di_1);
        c[i]=di/(di+di_1);
        d[i]=3.f/(di_1+di)*(npi-npi_1);
    }
	auto pc = tridiagonalMatrixAlgorithm(a,b,c,d);
    //calculate power polynomials
    std::vector<glm::vec3> pa(n-1), pb(n-1), pd(n-1);
    for(int i=0;i<n-1;i++) {
        float di = glm::length(getPoint(i+1)-getPoint(i));
        pa[i]=getPoint(i);
        pd[i]=(pc[i+1]-pc[i])/(di*3.f);
        pb[i]=(getPoint(i+1)-getPoint(i))/di-di/3.f*(2.f*pc[i]+pc[i+1]);
    }
    //normalise (make length of segment in parameter difference as 1)
    for(int i=0;i<n-1;i++) {
        float di = glm::length(getPoint(i+1)-getPoint(i));
        pb[i]*=di;
        pc[i]*=(di*di);
        pd[i]*=(di*di*di);
    }
    //convert to Bernstein base
    bezier.points.resize(3*n-2);
    for(int i=0;i<n-1;i++) {
        bezier.points[3*i]=pa[i];
        bezier.points[3*i+1]=pa[i]+pb[i]/3.f;
        bezier.points[3*i+2]=pa[i]+pb[i]/1.5f+pc[i]/3.f;
        if(i==n-2)
            bezier.points[3*i+3]=pa[i]+pb[i]+pc[i]+pd[i];
    }
    bezier.recalculate(wasSizeChanged);
    auto bezier2 = bf::bezier0ToBezier2(bezier.points);
    positions.clear();
    for(const auto& be: bezier2) {
        positions.emplace_back(be);
    }
	//update GPU data
	if(!wasSizeChanged) {
        bf::gl::namedBufferData(VBO, positions, isDynamic);
	}
	else {
		if(positions.empty()) {
			return;
		}
		std::vector<unsigned> tmpIndices;
		tmpIndices.reserve(positions.size()*2-2);
		for(int i=0;i<static_cast<int>(positions.size());i++) {
			if(i>0 && i<static_cast<int>(positions.size())-1)
				tmpIndices.emplace_back(i);
			tmpIndices.emplace_back(i);
		}
        if(VAO==UINT_MAX) {
            if(indices.empty() || vertices.empty()) {
                return;
            }
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            int usage = isDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;
            glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(Vertex), positions.data(), usage);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(0));
            glEnableVertexAttribArray(0);
            glGenBuffers(1, &IBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, tmpIndices.size() * sizeof(unsigned), tmpIndices.data(), usage);
        }
        else {
            bf::gl::namedBufferData(VBO, positions, isDynamic);
            bf::gl::namedBufferData(IBO, tmpIndices, isDynamic);
        }
	}
}

void bf::BezierCurveInter::draw(const bf::ShaderArray &shaderArray) const {
	if(pointIndices.size()>=2 && isPolygonVisible && shaderArray.getActiveIndex()==bf::ShaderType::BasicShader) {
        const Shader& shader = shaderArray.getActiveShader();
		shaderArray.setColor(64,64,192);
		shader.setMat4("model", glm::mat4(1.f));
		glBindVertexArray(VAO);
		glDrawElements(GL_LINES, positions.size() * 2 - 2, GL_UNSIGNED_INT, 0);
	}
    BezierCommon::draw(shaderArray);
}

bf::BezierCurveInter::~BezierCurveInter() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &IBO);
}
