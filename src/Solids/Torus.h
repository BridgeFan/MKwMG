//
// Created by kamil-hp on 15.03.2022.
//

#ifndef MG1_ZAD2_TORUS_H
#define MG1_ZAD2_TORUS_H
#include <vector>
#include <climits>
#include "Solid.h"
namespace bf {
	class Shader;
	class ObjectArray;
	class Torus : public bf::Solid {
		static int index;
		float bigRadius = 1.f, smallRadius = .3f;
		int bigFragments = 15;
		int smallFragments = 10;
		void updateTorus();
		friend bool saveToFile(const std::string &path, const bf::ObjectArray &objectArray);
	public:
		Torus(const bf::Transform &t, const std::string &torusName);
		Torus(const bf::Transform &t, const std::string &torusName,
			  float bigR, float smallR, int bigFrag, int smallFrag);
		explicit Torus(const bf::Transform &t = bf::Transform::Default);
		explicit Torus(const std::string &torusName);
		void ObjectGui() override;
	};
}


#endif //MG1_ZAD2_TORUS_H
