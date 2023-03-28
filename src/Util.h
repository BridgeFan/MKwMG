//
// Created by kamil-hp on 23.03.2022.
//

#ifndef MG1_ZAD2_UTIL_H
#define MG1_ZAD2_UTIL_H
#include <string>
#include <glm/glm.hpp>
#include <vector>
#include <memory>

class ImGuiIO;
namespace bf {
    struct Settings;
	class Scene;
    class Transform;
    struct GlfwStruct {
        bf::Settings &settings;
		bf::Scene &scene;
        const float &deltaTime;
        ImGuiIO &io;

        GlfwStruct(bf::Settings &settings1, bf::Scene& scene1, const float &deltaTime1, ImGuiIO &io1);
    };
}

std::string readWholeFile(const char* path);

std::string toString(const glm::vec3& v);
std::string toString(const glm::vec4& v);
bool isnan(const glm::vec3& v);
bool isnan(const glm::vec4& v);

bool almostEqual(float a1, float a2, float eps=1e-7);

#endif //MG1_ZAD2_UTIL_H
