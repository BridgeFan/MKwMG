//
// Created by kamil-hp on 16.03.2022.
//

#include "Solid.h"
#include <GL/glew.h>
#include "src/Shader/ShaderArray.h"
int bf::Solid::sindex = 1;
unsigned oldVerticesSize = 0;

bf::Solid::~Solid() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &IBO);
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
        int usage = isDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;
        glNamedBufferData(VBO, vertices.size() * sizeof(Vertex), vertices.data(), usage);
        glNamedBufferData(IBO, indices.size() * sizeof(unsigned), indices.data(), usage);
    }
}

void bf::Solid::draw(const bf::ShaderArray& shaderArray/*, const bf::Transform& relativeTo*/) const {
    if(indices.empty() || vertices.empty() || shaderArray.getActiveIndex()!=bf::ShaderType::BasicShader)
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
    std::swap(a.indices, b.indices);
    std::swap(a.vertices, b.vertices);
    std::swap(a.isDynamic, b.isDynamic);
    std::swap(a.indestructibilityIndex, b.indestructibilityIndex);
    std::swap(a.transform, b.transform);
    std::swap(a.name, b.name);
}

void bf::Solid::glUpdateVertices() const {
    if(oldVerticesSize!=vertices.size()) {
        int usage = isDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;
        glNamedBufferData(VBO, vertices.size() * sizeof(Vertex), vertices.data(), usage);
        oldVerticesSize=vertices.size();
    }
    else {
        glNamedBufferSubData(VBO, 0, vertices.size() * sizeof(Vertex), vertices.data());
    }
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

bf::DummySolid::DummySolid(const std::string &solidName, bool dynamic) : Solid(solidName, dynamic) {}

bf::DummySolid &bf::DummySolid::operator=(bf::DummySolid &&solid) noexcept {
    swapSolids(*this, solid);
    return *this;
}
