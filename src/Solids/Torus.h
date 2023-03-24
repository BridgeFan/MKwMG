//
// Created by kamil-hp on 15.03.2022.
//

#ifndef MG1_ZAD2_TORUS_H
#define MG1_ZAD2_TORUS_H
#include <vector>
#include <climits>
#include "Solid.h"

class Shader;
class Settings;

class Torus: public Solid {
	static int index;
	float bigRadius=1.f, smallRadius=.3f;
	int bigFragments = 15;
	int smallFragments = 10;
	void updateTorus();
public:
	Torus(const Transform& transform, const std::string& name): Solid(transform, name) {updateTorus();}
	explicit Torus(const Transform& transform=Transform::Default): Torus(transform, "Torus "+std::to_string(index)) {index++;updateTorus();}
	explicit Torus(const std::string& name): Torus(Transform::Default, name) {updateTorus();}
	void ObjectGui() override;
};


#endif //MG1_ZAD2_TORUS_H
