//
// Created by kamil-hp on 16.03.2022.
//

#include "Solid.h"
#include <GL/glew.h>
#include "src/Shader/ShaderArray.h"
int bf::Solid::sindex = 1;

bf::Solid::~Solid() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &IBO);
}

void bf::Solid::setBuffers() {
	//remove old ones
	if(VAO<UINT_MAX)
		glDeleteVertexArrays(1, &VAO);
	if(VBO<UINT_MAX)
		glDeleteBuffers(1, &VBO);
	if(IBO<UINT_MAX)
		glDeleteBuffers(1, &IBO);
	//set buffers
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

/*void bf::Solid::draw(const bf::ShaderArray& shaderArray) const {
    draw(shaderArray, Transform::Default);
}*/

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
	vertices.push_back(p);
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

bf::Vertex::Vertex(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

bf::Vertex::Vertex(const glm::vec3 &p): Vertex(p.x,p.y,p.z) {

}

bf::DummySolid::DummySolid(const std::string &solidName, bool dynamic) : Solid(solidName, dynamic) {}

bf::DummySolid &bf::DummySolid::operator=(bf::DummySolid &&solid) noexcept {
    swapSolids(*this, solid);
    return *this;
}
