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
		void draw(const ShaderArray &shader) const override;
		void ObjectGui() override;
        Torus(Torus&)=delete;
        Torus(Torus&& solid) noexcept;
        Torus operator=(const Torus&)=delete;
        Torus& operator=(bf::Torus&& solid) noexcept;
		void onMergePoints(int, int) override {}
		[[nodiscard]] bool isIntersectable() const override {return true;}
		[[nodiscard]] vec2d getParameterMin() const override {return {0.,0.};}
		[[nodiscard]] vec2d getParameterMax() const override {return {2.*M_PI,2.*M_PI};}
		[[nodiscard]] bool parameterWrappingU() const override {return true;}
		[[nodiscard]] bool parameterWrappingV() const override {return true;}
		[[nodiscard]] vec3d parameterFunction(double u, double v) const override;
		[[nodiscard]] vec3d parameterGradientU(double u, double v) const override;
		[[nodiscard]] vec3d parameterGradientV(double u, double v) const override;
	};
}


#endif //MG1_ZAD2_TORUS_H
