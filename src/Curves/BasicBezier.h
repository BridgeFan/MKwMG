#pragma once
//
// Created by kamil-hp on 18.04.23.
//

#ifndef MG1_ZAD2_BASICBEZIER_H
#define MG1_ZAD2_BASICBEZIER_H

#include <vector>
#include <glm/vec3.hpp>
#include "Solids/Solid.h"

namespace bf {
    class Scene;
    struct ConfigState;
	struct ShaderArray;
    class BasicBezier: bf::Solid {
    public:
        virtual ~BasicBezier()=default;
        BasicBezier();
        std::vector<glm::vec3> points;
        virtual void bezierDraw(const bf::ShaderArray &shaderArray, bool isLineDrawn, bool isPointDraw) const;
        void recalculate(bool wasSizeChanged=true);
    };
	std::vector<glm::vec3> bezier2ToBezier0(const std::vector<glm::vec3>& points);
	std::vector<glm::vec3> bezier0ToBezier2(const std::vector<glm::vec3>& points);
}


#endif //MG1_ZAD2_BASICBEZIER_H
