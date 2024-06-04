#pragma once
//
// Created by kamil-hp on 06.05.23.
//

#ifndef MG1_ZAD2_BEZIERSURFACE0_H
#define MG1_ZAD2_BEZIERSURFACE0_H

#include "BezierSurfaceCommon.h"

namespace bf {
	class ObjectArray;
    class Cursor;
	class Camera;

    class BezierSurface0: public bf::BezierSurfaceCommon {
        friend bool loadFromFile(bf::ObjectArray &objectArray, bf::Camera& camera, const std::string &path);
        static int _index;
    public:
        BezierSurface0(ObjectArray &objectArray, const std::string &objName, const Cursor &c);
        BezierSurface0(ObjectArray &objectArray, const Cursor &c);
        virtual std::vector<std::vector<pArray>> generatePoints(const glm::vec2& totalSize) override;
		[[nodiscard]] glm::vec3 parameterFunction(float u, float v) const override;
		[[nodiscard]] glm::vec3 parameterGradientU(float u, float v) const override;
		[[nodiscard]] glm::vec3 parameterGradientV(float u, float v) const override;
	};
}


#endif //MG1_ZAD2_BEZIERSURFACE0_H
