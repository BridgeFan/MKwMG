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

int Torus::index = 1;
constexpr float PI = std::numbers::pi_v<float>;
#ifndef M_PI
#define M_PI 3.14159265f
#endif

void Torus::updateTorus() {
	//generate vertices and indices
	vertices.clear();
	indices.clear();
	for(unsigned i=0u;i<(unsigned)smallFragments;i++) {
		float alpha = PI * 2.f * i / smallFragments;
		for(unsigned j=0u;j<(unsigned)bigFragments;j++) {
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

void Torus::ObjectGui() {
	Solid::ObjectGui();
	bool isCalculationNeeded=false;
	if(checkChanged("R",bigRadius)) {
		if(bigRadius<1e-6f)
			bigRadius=1e-6f;
		isCalculationNeeded=true;
	}
	if(checkChanged("r",smallRadius)) {
		if(smallRadius<.0f)
			smallRadius=.0f;
		isCalculationNeeded=true;
	}
	if(checkSliderChanged("R fragments",bigFragments, 3, 60)) {
		if(smallRadius<.0f)
			smallRadius=.0f;
		isCalculationNeeded=true;
	}
	if(checkSliderChanged("r fragments",smallFragments, 3, 60)) {
		if(smallRadius<.0f)
			smallRadius=.0f;
		isCalculationNeeded=true;
	}
	if(isCalculationNeeded) {
		updateTorus();
	}
}
