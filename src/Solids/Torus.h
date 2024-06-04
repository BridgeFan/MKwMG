#pragma once
//
// Created by kamil-hp on 15.03.2022.
//

#ifndef MG1_ZAD2_TORUS_H
#define MG1_ZAD2_TORUS_H
#include "Solid.h"
namespace bf {
	struct Shader;
	class ObjectArray;
	class Camera;
	class Torus : public bf::Solid {
		static int index;
		void updateTorus();
        void swapTori(Torus& a, Torus& b);
		friend bool saveToFile(const bf::ObjectArray &objectArray, bf::Camera& camera, const std::string &path);
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
		void onMergePoints(int, int) override {}
		[[nodiscard]] bool isIntersectable() const override {return true;}
		[[nodiscard]] glm::vec2 getParameterMin() const override {return {0.f,0.f};}
		[[nodiscard]] glm::vec2 getParameterMax() const override {return {M_2_PI,M_2_PI};}
		[[nodiscard]] bool parameterWrappingU() const override {return true;}
		[[nodiscard]] bool parameterWrappingV() const override {return true;}
		[[nodiscard]] glm::vec3 parameterFunction(float u, float v) const override;
		[[nodiscard]] glm::vec3 parameterGradientU(float u, float v) const override;
		[[nodiscard]] glm::vec3 parameterGradientV(float u, float v) const override;
	};
}


#endif //MG1_ZAD2_TORUS_H
