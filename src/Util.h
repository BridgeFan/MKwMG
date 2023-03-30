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
	template<typename T>
	concept arithmetic = std::is_arithmetic<T>::value;
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
template<bf::arithmetic T, std::integral U>
T fastPow(T base, U power) {
	if(power<0) {
		power = -power;
		base = static_cast<T>(1)/base;
	}
	T result = static_cast<T>(1);
	while(power > 1) {
		if(power & 1) { // Can also use (power & 1) to make code even faster
			result *= base;
		}
		base *= base;
		power >>= 1; // Can also use power >>= 1; to make code even faster
	}
	return base*result;
}

bool almostEqual(float a1, float a2, float eps=1e-7);

#endif //MG1_ZAD2_UTIL_H
