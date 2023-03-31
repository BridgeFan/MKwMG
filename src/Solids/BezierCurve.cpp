//
// Created by kamil-hp on 27.03.23.
//

#include <algorithm>
#include <GL/glew.h>
#include "BezierCurve.h"
#include "../Object/ObjectArray.h"
#include "Point.h"
#include "ImGuiUtil.h"
#include "imgui-master/imgui.h"
#include "../Shader.h"
#include "Util.h"
#include "GlfwUtil.h"
#include "Scene.h"

int bf::BezierCurve::_index = 1;
const bf::Scene* bf::BezierCurve::scene = nullptr;
const bf::Settings* bf::BezierCurve::settings = nullptr;
GLFWwindow* bf::BezierCurve::window = nullptr;

int getIndex(unsigned lod) {
	//begin
	return fastPow(2,lod+1u)-2;
}
inline unsigned countParts(unsigned i) {return (i+1)/3;}

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

bool bf::BezierCurve::addPoint(unsigned index) {
	if(!objectArray.isCorrect(index) || typeid(objectArray[index])!=typeid(bf::Point)) {
        return false;
    }
    else if(std::find(pointIndices.begin(), pointIndices.end(),index)!=pointIndices.end()) {
        return false;
    }
	pointIndices.push_back(index);
	recalculate();
	return true;
}

bool bf::BezierCurve::removePoint(unsigned index) {
	if(!objectArray.isCorrect(index) || pointIndices.empty())
		return false;
	for(std::size_t i=index;i<objectArray.size();i++)
		std::swap(pointIndices[i], pointIndices[i + 1u]);
	pointIndices.pop_back();
	activeIndex=0;
	recalculate();
	return true;
}

void bf::BezierCurve::onRemoveObject(unsigned index) {
	unsigned oldN = pointIndices.size();
	//remove object from list if it is removed by ObjectArray
	for(unsigned i=0u; i < pointIndices.size(); i++) {
		if(pointIndices[i] == index) {
			for(unsigned j=i+1;j<pointIndices.size();j++) {
				std::swap(pointIndices[j-1], pointIndices[j]);
			}
			pointIndices.pop_back();
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

bf::BezierCurve::BezierCurve(bf::ObjectArray &array):
	bf::Solid("Bézier curve0 " + std::to_string(_index)), ObjectArrayListener(array),
	isPolygonVisible(false), isCurveVisible(true) {
		_index++;
	for(std::size_t i=0;i<objectArray.size();i++) {
		if(objectArray.isActive(i) && typeid(objectArray[i])==typeid(bf::Point)) {
			pointIndices.push_back(i);
		}
	}
	recalculate();
}

void bf::BezierCurve::draw(const bf::Shader &shader) const {
    //draw points if active
    if(objectArray.isCorrect(objectArray.getActiveIndex()) && &objectArray[objectArray.getActiveIndex()]==this) {
        for(int i=0;i<static_cast<int>(pointIndices.size());i++) {
            if(i==activeIndex)
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
    if(fovIndices.empty() || fovVertices.empty()) {
        return;
    }
    shader.setVec3("color", 1.f,0.5f,0.f);
	if(isCurveVisible) {
        int LOD=0;
		glBindVertexArray(FVAO);
		for(unsigned i=0u;i<countParts(pointIndices.size());i++) {
			if(!scene || !window || !settings)
				LOD = 3;
			else {
				//bool pr = false;
				auto gPos1 = glm::vec2(bf::glfw::toScreenPos(window,objectArray[indices[i*3]].getPosition(),
												   scene->getView(), scene->getProjection()));
				auto gPos2 = glm::vec2(bf::glfw::toScreenPos(window,objectArray[indices[i*3+1]].getPosition(),
												   scene->getView(), scene->getProjection()));
				//pr |= bf::glfw::isInBounds(window, gPos1);
				//pr |= bf::glfw::isInBounds(window, gPos2);
				float distance = glm::distance(gPos1,gPos2);
				if(i+2<pointIndices.size()) {
					gPos1 = glm::vec2(bf::glfw::toScreenPos(window,objectArray[indices[i*3+2]].getPosition(),
																 scene->getView(), scene->getProjection()));
					//pr |= bf::glfw::isInBounds(window, gPos1);
					distance += glm::distance(gPos1, gPos2);
				}
				if(i+3<pointIndices.size()) {
					gPos2 = glm::vec2(bf::glfw::toScreenPos(window,objectArray[indices[i*3+3]].getPosition(),
															scene->getView(), scene->getProjection()));
					//pr |= bf::glfw::isInBounds(window, gPos1);
					distance += glm::distance(gPos1, gPos2);
				}
				if(i+2>=pointIndices.size()/* || !pr*/)
					LOD=0;
				else {
					float tmp=1.f;
					for(LOD=0;i<MAX_FOV_LOG_PARTS;LOD++) {
						if(distance<=tmp*3.f)
							break;
						tmp*=2.f;
					}
				}
			}
			glDrawElements(GL_LINES, fastPow(2,LOD+1), GL_UNSIGNED_INT,   // type
						   reinterpret_cast<void *>((getIndex(LOD) + i*getIndex(MAX_FOV_LOG_PARTS+1)) * sizeof(GLuint))           // element array buffer offset
			);
		}
	}
}

void bf::BezierCurve::ObjectGui() {
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

glm::vec3 deCasteljau(float t, const std::vector<glm::vec3>& pos) {
	unsigned n = pos.size();
	auto beta = pos;
	for(unsigned i=1;i<=n;i++) {
		for(unsigned k=0;k<n-i;k++) {
			beta[k] = beta[k]*(1-t)+beta[k+1]*t;
		}
	}
	return beta[0];
}

void bf::BezierCurve::recalculate() {
	indices.clear();
	vertices.clear();
	fovVertices.clear();
	fovIndices.clear();
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
	fovVertices.reserve(MAX_FOV_PARTS*countParts(pointIndices.size()));
	fovIndices.reserve(2*getIndex(MAX_FOV_LOG_PARTS+1)*countParts(pointIndices.size()));
	//update curve
	//vertices
	for(int i=0;i<static_cast<int>(pointIndices.size())-1;i+=3) {
		//set basic points for de Casteljau
		std::vector<glm::vec3> pos = {objectArray[pointIndices[i]].getPosition(),
									  objectArray[pointIndices[i + 1]].getPosition()};
		if (pointIndices.size() - i >= 3)
			pos.push_back(objectArray[pointIndices[i + 2]].getPosition());
		if (pointIndices.size() - i >= 4)
			pos.push_back(objectArray[pointIndices[i + 3]].getPosition());
		for (int k = 0; k <= MAX_FOV_PARTS; k++) {
			auto dcPos = deCasteljau(static_cast<float>(k) / MAX_FOV_PARTS, pos);
			fovVertices.emplace_back(dcPos);
		}
	}
	//indices
	for(unsigned k=0u;k<countParts(pointIndices.size()); k++) {
		//set indices for adaptivity
		int s = k * MAX_FOV_PARTS;
		int offset = MAX_FOV_PARTS;
		for(unsigned i=0u;i<=MAX_FOV_LOG_PARTS;i++)
		{
			fovIndices.push_back(s+0);
			for(int j=offset;j<MAX_FOV_PARTS;j+=offset) {
				fovIndices.push_back(s+j);
				fovIndices.push_back(s+j);
			}
			fovIndices.push_back(s+MAX_FOV_PARTS);
			offset/=2;
		}
	}
	setFovBuffers();
}
void bf::BezierCurve::recalculatePart(int /*index*/) {
    //assumes correctness of index
    //set basic points for de Casteljau
    /*std::vector<glm::vec3> pos = {objectArray[pointIndices[3*index]].getPosition(),
                                  objectArray[pointIndices[3*index + 1]].getPosition()};
    if (pointIndices.size() - index*3 >= 3)
        pos.push_back(objectArray[pointIndices[index + 2]].getPosition());
    if (pointIndices.size() - index*3 >= 4)
        pos.push_back(objectArray[pointIndices[index + 3]].getPosition());
    for (int k = 0; k <= MAX_FOV_PARTS; k++) {
        auto dcPos = deCasteljau(static_cast<float>(k) / MAX_FOV_PARTS, pos);
        fovVertices[(index*MAX_FOV_PARTS+k)*3]=dcPos.x;
        fovVertices[(index*MAX_FOV_PARTS+k)*3+1]=dcPos.y;
        fovVertices[(index*MAX_FOV_PARTS+k)*3+2]=dcPos.z;
    }
    glBindBuffer(GL_ARRAY_BUFFER, FVBO);
    glBufferSubData(GL_ARRAY_BUFFER, MAX_FOV_PARTS * sizeof(float) * 3, sizeof(float) * 3,&fovVertices[index*MAX_FOV_PARTS*3]);*/
	recalculate();
}

void bf::BezierCurve::onMoveObject(unsigned index) {
    auto f = std::find(pointIndices.begin(),pointIndices.end(),index);
	if(f !=pointIndices.end()) {
        auto piIndex = static_cast<int>(f-pointIndices.begin());
        if(piIndex%3==0) {
            recalculatePart(piIndex/3-1);
        }
        if(piIndex%3!=0 || piIndex<static_cast<int>(pointIndices.size())-1)
            recalculatePart(piIndex/3);
    }
}

void bf::BezierCurve::setFovBuffers() {
	//remove old ones
	if(FVAO<UINT_MAX)
		glDeleteVertexArrays(1, &FVAO);
	if(FVBO<UINT_MAX)
		glDeleteBuffers(1, &FVBO);
	if(FIBO<UINT_MAX)
		glDeleteBuffers(1, &FIBO);
	//set buffers
    if(fovIndices.empty() || fovVertices.empty()) {
        return;
    }
	glGenVertexArrays(1, &FVAO);
	glGenBuffers(1, &FVBO);
	glGenBuffers(1, &FIBO);

	glBindVertexArray(FVAO);

	glBindBuffer(GL_ARRAY_BUFFER, FVBO);
	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(fovVertices.size()) * sizeof(Vertex), fovVertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), reinterpret_cast<void*>(0));
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, FIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(fovIndices.size()) * sizeof(unsigned), fovIndices.data(), GL_STATIC_DRAW);
}

void bf::BezierCurve::initData(const bf::Scene &sc, const bf::Settings &s, GLFWwindow* w) {
	scene=&sc;
	settings=&s;
	window=w;
}

std::vector<unsigned int> bf::BezierCurve::usedVectors() const {
    return pointIndices;
}
