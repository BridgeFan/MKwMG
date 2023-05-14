#pragma once
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
		void updateTorus();
        void swapTori(Torus& a, Torus& b);
		friend bool saveToFile(const bf::ObjectArray &objectArray, const std::string &path);
	public:
        float bigRadius = 1.f, smallRadius = .3f;
        int bigFragments = 15;
        int smallFragments = 10;
		Torus(const bf::Transform &t, const std::string &torusName);
		Torus(const bf::Transform &t, const std::string &torusName,
			  float bigR, float smallR, int bigFrag, int smallFrag);
		explicit Torus(const bf::Transform &t = bf::Transform::Default);
		explicit Torus(const std::string &torusName);
		void ObjectGui() override;
        Torus(Torus&)=delete;
        Torus(Torus&& solid) noexcept;
        Torus operator=(const Torus&)=delete;
        Torus& operator=(bf::Torus&& solid) noexcept;
	};
}


#endif //MG1_ZAD2_TORUS_H
