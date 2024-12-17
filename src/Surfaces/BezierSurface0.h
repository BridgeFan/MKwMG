#pragma once
//
// Created by kamil-hp on 06.05.23.
//

#ifndef MG1_ZAD2_BEZIERSURFACE0_H
#define MG1_ZAD2_BEZIERSURFACE0_H

#include "BezierSurfaceCommon.h"

#include <functional>

namespace bf {
	class ObjectArray;
    class Cursor;
	class Camera;

    class BezierSurface0: public bf::BezierSurfaceCommon {
        friend bool loadFromFile(bf::ObjectArray &objectArray, bf::Camera& camera, const std::string &path);
        static int _index;
		[[nodiscard]] bf::vec3d parameterFunction(double u, double v, int iu, int iv) const;
		[[nodiscard]] bf::vec3d parameterGradientU(double u, double v, int iu, int iv) const;
    	[[nodiscard]] bf::vec3d parameterGradientV(double u, double v, int iu, int iv) const;
    	[[nodiscard]] vec3d parameterHesseUU(double u, double v, int iu, int iv) const;
    	[[nodiscard]] vec3d parameterHesseUV(double u, double v, int iu, int iv) const;
    	[[nodiscard]] vec3d parameterHesseVV(double u, double v, int iu, int iv) const;
    public:
        BezierSurface0(ObjectArray &objectArray, const std::string &objName, const Cursor &c);
        BezierSurface0(ObjectArray &objectArray, const Cursor &c);
        virtual std::vector<std::vector<pArray>> generatePoints(const glm::vec2& totalSize) override;
		[[nodiscard]] bf::vec3d parameterFunction(double u, double v, int iu, int iv, const std::function<bf::vec4d(double)> &buFunc, const std::function<bf::vec4d(double)> &bvFunc) const;
		[[nodiscard]] bf::vec3d parameterFunction(double u, double v) const override;
		[[nodiscard]] bf::vec3d parameterGradientU(double u, double v) const override;
    	[[nodiscard]] bf::vec3d parameterGradientV(double u, double v) const override;
    	[[nodiscard]] vec3d parameterHesseUU(double u, double v) const override;
    	[[nodiscard]] vec3d parameterHesseUV(double u, double v) const override;
    	[[nodiscard]] vec3d parameterHesseVV(double u, double v) const override;
    	bf::vec3d setFunc(double u, double v, const std::function<bf::vec3d(double, double, int, int)>& func) const;
	};
}


#endif //MG1_ZAD2_BEZIERSURFACE0_H
