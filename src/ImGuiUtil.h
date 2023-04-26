//
// Created by kamil-hp on 13.03.2022.
//

#ifndef MG1_ZAD2_IMGUIUTIL_H
#define MG1_ZAD2_IMGUIUTIL_H
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <string>
#include <vector>

class ImGuiIO;
struct GLFWwindow;
namespace bf {
	namespace imgui {
		ImGuiIO &init(GLFWwindow *window);
		void destroy();
		bool checkChanged(const char *name, float &value);
		bool checkChanged(const char *name, float &value, float min, float max, float step = 1.f, float stepGreat = 10.f);
		bool checkChanged(const char *name, int &value);
		bool checkChanged(const char *name, int &value, int min, int max, int stepGreat = 10);
		bool checkChanged(const char *name, glm::vec3 &values, bool isZeroInsurance = false);
		bool checkChanged(const char *name, glm::vec2 &values, const char* format = nullptr);
		bool checkChanged(const char *name, std::string &value);
		bool checkSliderChanged(const char *name, int &value, int min, int max);
		bool checkSliderChanged(const char *name, float &value, float min, float max);
		bool checkSelectableChanged(const char *name, bool &selectable);
	}
}

#endif //MG1_ZAD2_IMGUIUTIL_H
