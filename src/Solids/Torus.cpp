//
// Created by kamil-hp on 15.03.2022.
//

#include "Torus.h"
#include "ConfigState.h"
#include "ImGui/ImGuiUtil.h"
#include "Shader/ShaderArray.h"
#include <GL/glew.h>
#include <numbers>

int bf::Torus::index = 1;
constexpr float PI = std::numbers::pi_v<float>;

void bf::Torus::updateTorus() {
	vertices.clear();
	indices.clear();
	if(vertices.empty()) {
		vertices.emplace_back();
	}
	indices.emplace_back(0u);
	//generate vertices and indices
	/*vertices.clear();
	indices.clear();
	for(unsigned i=0u;i<static_cast<unsigned>(smallFragments);i++) {
		float alpha = PI * 2.f * i / smallFragments;
		for(unsigned j=0u;j<static_cast<unsigned>(bigFragments);j++) {
			float beta = PI * 2.f * j / bigFragments;
			//x, y, z
			vertices.emplace_back((bigRadius + smallRadius * std::cos(alpha)) * std::cos(beta),
                                (bigRadius+smallRadius*std::cos(alpha))*std::sin(beta),
                                smallRadius*std::sin(alpha));
			//add indices
			indices.push_back(i*bigFragments+j);
			indices.push_back(i*bigFragments+((j+1)%bigFragments));
			indices.push_back(i*bigFragments+j);
			indices.push_back(((i+1)%smallFragments)*bigFragments+j);
		}
	}*/
	updateDebug();
	setBuffers();
}

void bf::Torus::ObjectGui() {
	bf::Solid::ObjectGui();
    if(!configState) {
        return;
    }
	bool isCalculationNeeded=false;
	if(bf::imgui::checkChanged("R",bigRadius)) {
		if(bigRadius<1e-6f)
			bigRadius=1e-6f;
		isCalculationNeeded=true;
	}
	if(bf::imgui::checkChanged("r",smallRadius)) {
		if(smallRadius<.0f)
			smallRadius=.0f;
		isCalculationNeeded=true;
	}
	if(bf::imgui::checkSliderChanged("R fragments",bigFragments, 3, configState->maxTorusFragments)) {
		if(smallRadius<.0f)
			smallRadius=.0f;
		isCalculationNeeded=true;
	}
	if(bf::imgui::checkSliderChanged("r fragments",smallFragments, 3, configState->maxTorusFragments)) {
		if(smallRadius<.0f)
			smallRadius=.0f;
		isCalculationNeeded=true;
	}
	if(isCalculationNeeded) {
		updateTorus();
	}
}

bf::Torus::Torus(const bf::Transform &t) : Torus(t, "Torus " + std::to_string(
		index)) {
	index++;
	updateTorus();
}

bf::Torus::Torus(const std::string &torusName) : Torus(bf::Transform::Default, torusName) { updateTorus(); }

bf::Torus::Torus(const bf::Transform &t, const std::string &torusName) : bf::Solid(t, torusName) { updateTorus(); }

bf::Torus::Torus(const bf::Transform &t, const std::string &torusName, float bigR, float smallR, int bigFrag,
				 int smallFrag): bf::Solid(t,torusName), bigRadius(bigR), smallRadius(smallR),
				 bigFragments(bigFrag), smallFragments(smallFrag) {
	updateTorus();
}

void bf::Torus::swapTori(bf::Torus &a, bf::Torus &b) {
    std::swap(a.smallRadius,b.smallRadius);
    std::swap(a.bigRadius,b.bigRadius);
    std::swap(a.smallFragments,b.smallFragments);
    std::swap(a.bigFragments,b.bigFragments);
}

bf::Torus::Torus(bf::Torus &&solid) noexcept: bf::Solid(std::move(solid)) {
    swapTori(*this, solid);
}

bf::Torus &bf::Torus::operator=(bf::Torus &&solid) noexcept {
    swapSolids(*this, solid);
    swapTori(*this, solid);
    return *this;
}
bf::vec3d bf::Torus::parameterFunction(double uf, double vf) const {
	bf::vec4d param = clampParam(uf,vf);
	double u = param.x;
	double v = param.y;
	bf::vec4d vector = {(bigRadius + smallRadius * std::cos(u)) * std::cos(v),
			(bigRadius+smallRadius*std::cos(u))*std::sin(v),
			smallRadius*std::sin(u), 1.f};
	return getModelMatrix()*vector;
}
bf::vec3d bf::Torus::parameterGradientU(double uf, double vf) const {
	bf::vec4d param = clampParam(uf,vf);
	double u = param.x;
	double v = param.y;
	bf::vec4d vector = {(-smallRadius*std::sin(u)) * std::cos(v),
						(-smallRadius*std::sin(u))*std::sin(v),
						smallRadius*std::cos(u), 0.f};
	return getModelMatrix()*vector;
}
bf::vec3d bf::Torus::parameterGradientV(double uf, double vf) const {
	bf::vec4d param = clampParam(uf, vf);
	double u = param.x;
	double v = param.y;
	bf::vec4d vector = {-(bigRadius + smallRadius * std::cos(u)) * std::sin(v),
						(bigRadius + smallRadius * std::cos(u)) * std::cos(v),
						0.f, 0.f};
	return getModelMatrix() * vector;
}
bf::vec3d bf::Torus::parameterHesseUU(double uf, double vf) const {
	bf::vec4d param = clampParam(uf,vf);
	double u = param.x;
	double v = param.y;
	bf::vec4d vector = {(-smallRadius*std::cos(u)) * std::cos(v),
						(-smallRadius*std::cos(u))*std::sin(v),
						-smallRadius*std::sin(u), 0.f};
	return getModelMatrix()*vector;
}
bf::vec3d bf::Torus::parameterHesseUV(double uf, double vf) const {
	bf::vec4d param = clampParam(uf,vf);
	double u = param.x;
	double v = param.y;
	bf::vec4d vector = {(smallRadius*std::sin(u)) * std::sin(v),
						(-smallRadius*std::sin(u))*std::cos(v),
						0.0, 0.f};
	return getModelMatrix()*vector;
}
bf::vec3d bf::Torus::parameterHesseVV(double uf, double vf) const {
	bf::vec4d param = clampParam(uf, vf);
	double u = param.x;
	double v = param.y;
	bf::vec4d vector = {-(bigRadius + smallRadius * std::cos(u)) * std::cos(v),
						-(bigRadius + smallRadius * std::cos(u)) * std::sin(v),
						0.f, 0.f};
	return getModelMatrix() * vector;
}

std::pair<glm::vec3, glm::vec3> bf::Torus::getObjectRange() const {
	//TODO: better
	auto V = smallRadius + bigRadius;
	glm::vec3 min = transform.position - glm::vec3(V,V,V);
	glm::vec3 max = transform.position + glm::vec3(V,V,V);
	return {min, max};
}
void bf::Torus::draw(const bf::ShaderArray &shaderArray) const {
	drawDebug(shaderArray, false);
	if(shaderArray.getActiveIndex()!=bf::ShaderType::TorusShader) {
		return;
	}
	if(indices.empty() || vertices.empty())
		return;
	//function assumes set projection and view matrices
	glBindVertexArray(VAO);
	shaderArray.setUniform("textureMode", textureMode);
	if(textureMode>0 && textureID<1000000) {
		glBindTexture(GL_TEXTURE_2D, textureID);
	}
	const auto& shader = shaderArray.getActiveShader();
	shader.setMat4("model", getModelMatrix(/*relativeTo*/));
	shader.setInt("SegmentsX", smallFragments);
	shader.setInt("SegmentsY", bigFragments);
	shader.setFloat("R", bigRadius);
	shader.setFloat("r", smallRadius);

	/*glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT,   // type
				   reinterpret_cast<void*>(0)           // element array buffer offset
	);*/
	glDrawElements(GL_PATCHES, indices.size(), GL_UNSIGNED_INT, reinterpret_cast<void*>(0));
	shaderArray.setUniform("textureMode", 0);
	/*if(shaderArray.getActiveIndex()!=bf::ShaderType::BasicShader) {
		return;
	}
	anyDraw(shaderArray);*/
}
