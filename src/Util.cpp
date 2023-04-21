//
// Created by kamil-hp on 23.03.2022.
//
#include <memory>
#include "Util.h"
#include "Settings.h"
#include "Scene.h"

std::string toString(const glm::vec3& v) {
	return "("+std::to_string(v.x)+","+std::to_string(v.y)+", "+std::to_string(v.z)+")";
}

std::string toString(const glm::vec4& v) {
	return "("+std::to_string(v.x)+","+std::to_string(v.y)+", "+std::to_string(v.z)+","+std::to_string(v.w)+")";
}

std::string readWholeFile(const char* path) {
    FILE* f = fopen(path, "r");
    if(f==nullptr)
        return "";
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);  /* same as rewind(f); */

    char *string = new char[fsize + 1];
    fread(string, fsize, 1, f);
    fclose(f);

    string[fsize] = 0;
    std::string ret(string);
    delete[] string;
    return ret;
}

bool isnan(const glm::vec3 &v) {
    return std::isnan(v.x) || std::isnan(v.y) || std::isnan(v.z);
}
bool isnan(const glm::vec4 &v) {
    return std::isnan(v.x) || std::isnan(v.y) || std::isnan(v.z) || std::isnan(v.w);
}

bool almostEqual(float a1, float a2, float eps) {
	return std::abs(a1-a2)<eps*std::max(std::max(std::abs(a1),std::abs(a2)),1e-5f);
}
bf::GlfwStruct::GlfwStruct(bf::Settings &settings1, bf::Scene& scene1, const float &deltaTime1, ImGuiIO &io1) : settings(settings1), scene(scene1),
						   	deltaTime(deltaTime1), io(io1) {}
