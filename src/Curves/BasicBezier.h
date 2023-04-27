//
// Created by kamil-hp on 18.04.23.
//

#ifndef MG1_ZAD2_BASICBEZIER_H
#define MG1_ZAD2_BASICBEZIER_H

#include <vector>
#include <glm/vec3.hpp>
#include "Solids/Solid.h"

//constexpr int MAX_FOV_PARTS = 256;
//constexpr int MAX_FOV_LOG_PARTS = 8; //should be equal to log2(MAX_FOV_PARTS)

struct GLFWwindow;
namespace bf {
    class Scene;
    class Settings;
	class ShaderArray;
    class BasicBezier: bf::Solid {
    public:
        virtual ~BasicBezier()=default;
        BasicBezier();
        std::vector<glm::vec3> points;
        virtual void draw(const bf::ShaderArray& shaderArray, GLFWwindow* window, const bf::Scene& scene, const bf::Settings& settings, bool isLineDrawn=false, bool isPointDraw=false) const;
        void recalculate(bool wasSizeChanged=true);
    };
	std::vector<glm::vec3> bezier2ToBezier0(const std::vector<glm::vec3>& points);
	std::vector<glm::vec3> bezier0ToBezier2(const std::vector<glm::vec3>& points);
}


#endif //MG1_ZAD2_BASICBEZIER_H
