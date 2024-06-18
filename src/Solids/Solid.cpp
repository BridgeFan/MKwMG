//
// Created by kamil-hp on 16.03.2022.
//

#include "Solid.h"
#include "Util.h"
#include "src/ConfigState.h"
#include "src/Shader/ShaderArray.h"
#include <GL/glew.h>
#include <OpenGLUtil.h>
#include <iostream>
int bf::Solid::sindex = 1;
unsigned oldVerticesSize = 0;

bf::Solid::~Solid() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &IBO);
	if(debugVAO!=UINT_MAX) {
		glDeleteVertexArrays(1, &debugVAO);
		glDeleteBuffers(1, &debugVBO);
		glDeleteBuffers(1, &debugIBO);
	}
}

void bf::Solid::setBuffers() {
    oldVerticesSize = vertices.size();
	//remove old ones
    if(VAO==UINT_MAX) {
        if(indices.empty() || vertices.empty()) {
            return;
        }
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        int usage = isDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), usage);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(0));
        glEnableVertexAttribArray(0);
        glGenBuffers(1, &IBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned), &indices[0], usage);
    }
    else {
        bf::gl::namedBufferData(VBO, vertices, isDynamic);
        bf::gl::namedBufferData(IBO, indices, isDynamic);
    }
}

void bf::Solid::draw(const bf::ShaderArray& shaderArray) const {
	drawDebug(shaderArray);
    if(shaderArray.getActiveIndex()!=bf::ShaderType::BasicShader) {
		return;
	}
    anyDraw(shaderArray);
}

void bf::Solid::anyDraw(const bf::ShaderArray& shaderArray/*, const bf::Transform& relativeTo*/) const {
    if(indices.empty() || vertices.empty())
        return;
    //function assumes set projection and view matrices
    glBindVertexArray(VAO);
    shaderArray.getActiveShader().setMat4("model", getModelMatrix(/*relativeTo*/));
    glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT,   // type
                   reinterpret_cast<void*>(0)           // element array buffer offset
    );
}

void bf::Solid::ObjectGui() {
	Object::ObjectGui();
}

void bf::Solid::addVertex(const glm::vec3 &p) {
	vertices.emplace_back(p);
}

bf::ShaderType bf::Solid::getShaderType() const {
    return BasicShader;
}

bf::Solid::Solid(bf::Solid &&solid) noexcept {
    swapSolids(*this, solid);
}

void bf::Solid::swapSolids(bf::Solid &a, bf::Solid &b) {
    if(&a==&b)
        return;
    std::swap(a.VAO, b.VAO);
    std::swap(a.VBO, b.VBO);
    std::swap(a.IBO, b.IBO);
	std::swap(a.debugVAO, b.debugVAO);
	std::swap(a.debugVBO, b.debugVBO);
	std::swap(a.debugIBO, b.debugIBO);
	std::swap(a.debugN, b.debugN);
    std::swap(a.indices, b.indices);
    std::swap(a.vertices, b.vertices);
    std::swap(a.isDynamic, b.isDynamic);
    std::swap(a.indestructibilityIndex, b.indestructibilityIndex);
    std::swap(a.transform, b.transform);
    std::swap(a.name, b.name);
}

void bf::Solid::glUpdateVertices() const {
    if(oldVerticesSize!=vertices.size()) {
        bf::gl::namedBufferData(VBO, vertices, isDynamic);
        oldVerticesSize=vertices.size();
    }
    else {
        bf::gl::namedBufferSubData(VBO, vertices, 0, vertices.size());
    }
}
std::pair<std::vector<bf::Vertex>, std::vector<unsigned>> bf::Solid::createDebugInfo(int n) const {
	std::vector<bf::Vertex> vert;
	std::vector<unsigned> indicesPts, indicesLines;
	n=std::max(n,1);
	vert.reserve(3*n*n);
	indicesPts.reserve(n*n);
	indicesLines.reserve(4*n*n);
	auto nf= static_cast<float>(n);
	auto pmin = getParameterMin();
	auto pmax = getParameterMax();
	for(int i=0;i<=n;i++) {
		float u=lerp(pmin[0],pmax[0],static_cast<float>(i)/nf);
		for(int j=0;j<=n;j++) {
			float v=lerp(pmin[1],pmax[1],static_cast<float>(j)/nf);
			unsigned k=vert.size();
			auto p=parameterFunction(u,v);
			auto pu=parameterGradientU(u,v);
			auto pv=parameterGradientV(u,v);
			vert.emplace_back(p);
			vert.emplace_back(p+glm::normalize(pu)*0.1);
			vert.emplace_back(p+glm::normalize(pv)*0.1);
			indicesPts.push_back(k);
			indicesLines.insert(indicesLines.end(),{k,k+1,k,k+2});
		}
	}
	indicesPts.insert(indicesPts.end(), indicesLines.begin(), indicesLines.end());
	return {vert, indicesPts};
}

void bf::Solid::setDebugBuffers(const std::vector<bf::Vertex> &vert, const std::vector<unsigned> &ind, bool areIndicesSet) {
	oldVerticesSize = vert.size();
	debugN=vert.size()/3;
	//remove old ones
	if(debugVAO==UINT_MAX) {
		if(ind.empty() || vert.empty()) {
			return;
		}
		glGenVertexArrays(1, &debugVAO);
		glGenBuffers(1, &debugVBO);
		glBindVertexArray(debugVAO);
		glBindBuffer(GL_ARRAY_BUFFER, debugVBO);
		int usage = GL_DYNAMIC_DRAW;
		glBufferData(GL_ARRAY_BUFFER, vert.size() * sizeof(Vertex), vert.data(), usage);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(0));
		glEnableVertexAttribArray(0);
		glGenBuffers(1, &debugIBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, debugIBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind.size() * sizeof(unsigned), ind.data(), usage);
	}
	else {
		if(areIndicesSet) {
			bf::gl::namedBufferData(debugVBO, vert, isDynamic);
			bf::gl::namedBufferData(debugIBO, ind, isDynamic);
		}
		else {
			bf::gl::namedBufferSubData(debugVBO, vert, 0, vert.size());
		}
	}
}
void bf::Solid::updateDebug(int N) {
	static int oldN=N;
	//debug
	if(configState && configState->isDebug) {
		auto&& [v, i] = createDebugInfo(N);
		setDebugBuffers(v,i, oldN!=N);
	}
	oldN=N;
}

void bf::Solid::drawDebug(const bf::ShaderArray& shaderArray, bool isModelUsed) const {
	bool isDebugActive = configState && configState->isDebug && debugVAO!=UINT_MAX && debugN>0;
	if(!isDebugActive || (shaderArray.getActiveIndex()!=bf::ShaderType::BasicShader)) return;
	auto color = shaderArray.getColor();
	shaderArray.setColor(255,255,0);
	glBindVertexArray(debugVAO);
	shaderArray.getActiveShader().setMat4("model", isModelUsed ? getModelMatrix(/*relativeTo*/) : glm::mat4(1.f));
	shaderArray.getActiveShader().setFloat("pointSize", 2.f*configState->pointRadius);
	glDrawElements(GL_POINTS, debugN, GL_UNSIGNED_INT,   // type
					   reinterpret_cast<void*>(0));         // element array buffer offset
	glDrawElements(GL_LINES, 4*debugN, GL_UNSIGNED_INT,   // type
					   reinterpret_cast<void*>(debugN*sizeof(unsigned))           // element array buffer offset
		);
	shaderArray.setColor(color);
}

bf::Vertex::Vertex(float _x, float _y, float _z, float _tX, float _tY) noexcept : x(_x), y(_y), z(_z), tX(_tX), tY(_tY) {}

bf::Vertex::Vertex(const glm::vec3 &p, const glm::vec2 &t) noexcept : Vertex(p.x,p.y,p.z,t.x,t.y) {

}

void bf::Vertex::setPosition(const glm::vec3 &p) noexcept {
    x = p.x; y = p.y; z = p.z;
}

void bf::Vertex::setTexturePosition(const glm::vec2 &t) noexcept {
    tX = t.x; tY = t.y;
}
bf::Vertex::Vertex() noexcept : Vertex(.0f,.0f,.0f) {}

bf::DummySolid::DummySolid(const std::string &solidName, bool dynamic) : Solid(solidName, dynamic) {}

bf::DummySolid &bf::DummySolid::operator=(bf::DummySolid &&solid) noexcept {
    swapSolids(*this, solid);
    return *this;
}
