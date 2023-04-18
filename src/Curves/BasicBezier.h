//
// Created by kamil-hp on 18.04.23.
//

#ifndef MG1_ZAD2_BASICBEZIER_H
#define MG1_ZAD2_BASICBEZIER_H

#include <vector>
#include <glm/vec3.hpp>
#include "Shader.h"
#include "Solids/Solid.h"

constexpr int MAX_FOV_PARTS = 256;
constexpr int MAX_FOV_LOG_PARTS = 8; //should be equal to log2(MAX_FOV_PARTS)

struct GLFWwindow;
namespace bf {
    class Scene;
    class Settings;
    class BasicBezier: bf::Solid {
    public:
        BasicBezier();
        std::vector<glm::vec3> points;
        void draw(const bf::Shader& shader, GLFWwindow* window, const bf::Scene& scene, const bf::Settings& settings) const;
        void recalculate(bool wasSizeChanged=true);
    };
}


#endif //MG1_ZAD2_BASICBEZIER_H
