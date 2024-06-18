#pragma once
//
// Created by kamil-hp on 13.03.2022.
//

#ifndef MG1_ZAD2_IMGUIUTIL_H
#define MG1_ZAD2_IMGUIUTIL_H
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <string>
#include <vector>
#include <filesystem>

#define U8(_S) reinterpret_cast<const char*>(u8##_S)
#ifdef WIN32
using mstring = std::wstring;
#else
using mstring = std::string;
#endif
struct ImGuiIO;
struct GLFWwindow;
namespace bf {
    struct ConfigState;
	namespace imgui {
        class IO {
        public:
            ImGuiIO& io;
            IO(GLFWwindow* window, const bf::ConfigState& configState);
            ~IO();
        };
        void preDraw();
        void postDraw();
		bool checkChanged(const char *name, float &value);
		bool checkChanged(const char *name, float &value, float min, float max, float step = 1.f, float stepGreat = 10.f);
		bool checkChanged(const char *name, int &value);
		bool checkChanged(const char *name, int &value, int min, int max, int stepGreat = 10);
		bool checkChanged(const char *name, glm::vec3 &values, bool isZeroInsurance = false);
		bool checkChanged(const char *name, glm::vec2 &values, const char* format = nullptr);
		bool checkChanged(const char *name, glm::vec<2,int> &values);
		bool checkChanged(const char *name, std::string &value);
#ifdef WIN32
        bool checkChanged(const char *name, mstring &value);
#endif
		bool checkSliderChanged(const char *name, int &value, int min, int max);
		bool checkSliderChanged(const char *name, float &value, float min, float max);
		bool checkSelectableChanged(const char *name, int index, bool &selectable);
	}
}
mstring operator ""_m(const char* a, size_t);
mstring toMStr(const std::string& str);
std::string toStr(const mstring& mstr);
mstring pathToMStr(const std::filesystem::path& path);

#endif //MG1_ZAD2_IMGUIUTIL_H
