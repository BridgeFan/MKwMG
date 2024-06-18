#pragma once
//
// Created by kamil-hp on 06.05.23.
//

#ifndef MG1_ZAD2_BEZIERSURFACE2_H
#define MG1_ZAD2_BEZIERSURFACE2_H

#include "BezierSurfaceCommon.h"

namespace bf {
	class ObjectArray;
    class Cursor;

    class BezierSurface2: public bf::BezierSurfaceCommon {
        friend bool loadFromFile(bf::ObjectArray &objectArray, const std::string &path);
        static int _index;
    public:
        BezierSurface2(ObjectArray &objectArray, const std::string &objName, const Cursor &c);
        BezierSurface2(ObjectArray &objectArray, const Cursor &c);
        std::vector<std::vector<pArray>> generatePoints(const glm::vec2 &totalSize) override;
		[[nodiscard]] bf::vec3d parameterFunction(double u, double v) const override;
		[[nodiscard]] bf::vec3d parameterGradientU(double u, double v) const override;
		[[nodiscard]] bf::vec3d parameterGradientV(double u, double v) const override;
	};
}


#endif //MG1_ZAD2_BEZIERSURFACE2_H
