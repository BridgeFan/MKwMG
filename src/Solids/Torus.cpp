//
// Created by kamil-hp on 15.03.2022.
//

#include "Torus.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "../Shader.h"
#include "../Settings.h"
#include "../ImGuiUtil.h"
#include <numbers>

int bf::Torus::index = 1;
constexpr float PI = std::numbers::pi_v<float>;

void bf::Torus::updateTorus() {
	//generate vertices and indices
	vertices.clear();
	indices.clear();
	for(unsigned i=0u;i<static_cast<unsigned>(smallFragments);i++) {
		float alpha = PI * 2.f * i / smallFragments;
		for(unsigned j=0u;j<static_cast<unsigned>(bigFragments);j++) {
			float beta = PI * 2.f * j / bigFragments;
			//x, y, z
			vertices.push_back((bigRadius+smallRadius*std::cos(alpha))*std::cos(beta));
			vertices.push_back((bigRadius+smallRadius*std::cos(alpha))*std::sin(beta));
			vertices.push_back(smallRadius*std::sin(alpha));
			//add indices
			indices.push_back(i*bigFragments+j);
			indices.push_back(i*bigFragments+((j+1)%bigFragments));
			indices.push_back(i*bigFragments+j);
			indices.push_back(((i+1)%smallFragments)*bigFragments+j);
		}
	}
	setBuffers();
}

void bf::Torus::ObjectGui() {
	bf::Solid::ObjectGui();
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
	if(bf::imgui::checkSliderChanged("R fragments",bigFragments, 3, 60)) {
		if(smallRadius<.0f)
			smallRadius=.0f;
		isCalculationNeeded=true;
	}
	if(bf::imgui::checkSliderChanged("r fragments",smallFragments, 3, 60)) {
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
