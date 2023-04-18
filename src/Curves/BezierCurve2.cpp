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
const bf::Scene* bf::BezierCurve2::scene = nullptr;
const bf::Settings* bf::BezierCurve2::settings = nullptr;
GLFWwindow* bf::BezierCurve2::window = nullptr;

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

bool bf::BezierCurve2::addPoint(unsigned index) {
	if(!objectArray.isCorrect(index) || typeid(objectArray[index])!=typeid(bf::Point)) {
        return false;
    }
    else if(std::find(pointIndices.begin(), pointIndices.end(),index)!=pointIndices.end()) {
        return false;
    }
	pointIndices.push_back(index);
    recalculate();
    //TODO - bezier
    /*bezier.points.push_back(objectArray[index].getPosition());
	bezier.recalculate();*/
	return true;
}

bool bf::BezierCurve2::removePoint(unsigned index) {
    //TODO - bezier
	if(!objectArray.isCorrect(index) || pointIndices.empty())
		return false;
	for(std::size_t i=index;i<objectArray.size();i++) {
        std::swap(pointIndices[i], pointIndices[i + 1u]);
        //std::swap(bezier.points[i], bezier.points[i + 1u]);
    }
	pointIndices.pop_back();
    //bezier.points.pop_back();
	activeIndex=0;
    recalculate();
	return true;
}

void bf::BezierCurve2::onRemoveObject(unsigned index) {
	unsigned oldN = pointIndices.size();
	//remove object from list if it is removed by ObjectArray
	for(unsigned i=0u; i < pointIndices.size(); i++) {
		if(pointIndices[i] == index) {
			for(unsigned j=i+1;j<pointIndices.size();j++) {
				std::swap(pointIndices[j-1], pointIndices[j]);
                std::swap(bezier.points[j-1], bezier.points[j]);
			}
			pointIndices.pop_back();
            bezier.points.pop_back();
			i--;
		}
	}
	//recalculate whole curve
	if(pointIndices.size()<oldN && !pointIndices.empty()) {
        recalculate();
	}
	//rearrange indices
	for(auto& pointIndex : pointIndices)
		if(pointIndex > index)
			pointIndex--;
}

bf::BezierCurve2::BezierCurve2(bf::ObjectArray &array):
	bf::Solid("Bézier curve2 " + std::to_string(_index)), ObjectArrayListener(array),
	isPolygonVisible(false), isCurveVisible(true) {
		_index++;
	for(std::size_t i=0;i<objectArray.size();i++) {
		if(objectArray.isActive(i) && typeid(objectArray[i])==typeid(bf::Point)) {
            addPoint(i);
		}
	}
	recalculate();
}

void bf::BezierCurve2::draw(const bf::Shader &shader) const {
    //draw points if active
    if(objectArray.isCorrect(objectArray.getActiveIndex()) && &objectArray[objectArray.getActiveIndex()]==this) {
        for(int i=0;i<static_cast<int>(pointIndices.size());i++) {
            if(i==static_cast<int>(activeIndex))
                shader.setVec3("color", 1.f,0.f,0.f);
            else
                shader.setVec3("color", 0.f,1.f,0.f);
            objectArray[pointIndices[i]].draw(shader);
        }
    }
    shader.setVec3("color", 0.f,0.f,0.f);
    if(pointIndices.empty() || indices.empty() || vertices.empty()) {
        return;
    }
	//function assumes set projection and view matrices
	shader.setMat4("model", glm::mat4(1.f)); //transform is ignored

	//glDrawArrays(GL_TRIANGLES, 0, vertices.size()/3);
	if(isPolygonVisible) {
		glBindVertexArray(VAO);
		glDrawElements(GL_LINES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT,   // type
					   reinterpret_cast<void *>(0)           // element array buffer offset
		);
	}
    shader.setVec3("color", 1.f,0.5f,0.f);
    //TODO - uncomment
	/*if(isCurveVisible && scene && settings) {
        bezier.draw(shader,window,*scene,*settings);
	}*/
}

void bf::BezierCurve2::ObjectGui() {
    bf::imgui::checkChanged("Curve name", name);
    if (ImGui::Button("Delete point"))
        removePoint(activeIndex);
    int ari = objectArray.getActiveRedirector();
    bool isRedirected = objectArray.isCorrect(ari);
    if (isRedirected) {
        if (ImGui::Button("End adding"))
            objectArray.setActiveRedirector(nullptr);
    } else {
        if (ImGui::Button("Add points"))
            objectArray.setActiveRedirector(this);
    }
    if (ImGui::BeginTable("split", 2)) {
        ImGui::TableNextColumn();
        if (!isRedirected && activeIndex > 0) {
            if (ImGui::Button("Up")) {
                std::swap(pointIndices[activeIndex - 1], pointIndices[activeIndex]);
                //std::swap(bezier.points[activeIndex-1],bezier.points[activeIndex]);
                activeIndex--;
                recalculatePart(activeIndex);
            }
        } else {
            ImGui::Text("Up");
        }
        ImGui::TableNextColumn();
        if (!isRedirected && activeIndex < pointIndices.size() - 1) {
            if (ImGui::Button("Down")) {
                std::swap(pointIndices[activeIndex], pointIndices[activeIndex + 1]);
                //TODO - bezier
                //std::swap(bezier.points[activeIndex],bezier.points[activeIndex+1]);
                activeIndex++;
                recalculatePart(activeIndex);
            }
        } else {
            ImGui::Text("Down");
        }
        ImGui::EndTable();
    }
	ImGui::Checkbox("Polygon visible", &isPolygonVisible);
	ImGui::Checkbox("Curve visible", &isCurveVisible);
	ImGui::Text("List of points");
	for(unsigned i=0u; i < pointIndices.size(); i++) {
		if(!objectArray.isCorrect(pointIndices[i]))
			ImGui::Text("_");
		bool val = (i==activeIndex);
		bool ret = bf::imgui::checkSelectableChanged(objectArray[pointIndices[i]].name.c_str(), val);
		if(ret && val) {
			activeIndex=i;
		}
	}
}

void bf::BezierCurve2::recalculate() {
	indices.clear();
	vertices.clear();
	//set indices and vertices (default)
	for(unsigned i=0u;i<pointIndices.size();i++) {
		if(!objectArray.isCorrect(pointIndices[i]))
			continue;
		const auto& o = objectArray[pointIndices[i]];
		addVertex(o.getPosition());
		if(i<pointIndices.size()-1) {
			indices.push_back(i);
			indices.push_back(i+1);
		}
	}
	setBuffers();
    bezier.recalculate();
}
void bf::BezierCurve2::recalculatePart(int index) {
    //update vertices
    vertices.clear();
    for(unsigned i=0u;i<pointIndices.size();i++) {
        if(!objectArray.isCorrect(pointIndices[i]))
            continue;
        const auto& o = objectArray[pointIndices[i]];
        addVertex(o.getPosition());
    }
    glNamedBufferSubData(VBO, 0, vertices.size() * sizeof(Vertex), vertices.data());
    bezier.recalculate(false);
}

void bf::BezierCurve2::onMoveObject(unsigned index) {
    //TODO
    auto f = std::find(pointIndices.begin(),pointIndices.end(),index);
	if(f !=pointIndices.end()) {
        auto piIndex = static_cast<int>(f-pointIndices.begin());
        //bezier.points[piIndex]=objectArray[index].getPosition();
        if(piIndex%3==0) {
            recalculatePart(piIndex/3-1);
        }
        if(piIndex%3!=0 || piIndex<static_cast<int>(pointIndices.size())-1)
            recalculatePart(piIndex/3);
    }
}

void bf::BezierCurve2::initData(const bf::Scene &sc, const bf::Settings &s, GLFWwindow* w) {
	scene=&sc;
	settings=&s;
	window=w;
}

std::vector<unsigned int> bf::BezierCurve2::usedVectors() const {
    return pointIndices;
}

bool bf::BezierCurve2::onKeyPressed(int key, int) {
    if(objectArray.getActiveRedirector()>=0) {
        if(key==GLFW_KEY_P && scene) {
            objectArray.add<bf::Point>(scene->cursor.transform);
            addPoint(objectArray.size()-1);
            return true;
        }
        else if(key==GLFW_KEY_T) {
            return true;
        }
    }
    return false;
}
