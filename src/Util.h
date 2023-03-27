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
	class ObjectArray;
    class Camera;
    class Cursor;
    class MultiCursor;
    class Transform;
    struct GlfwStruct {
        bf::Settings &settings;
        bf::Camera &camera;
        //std::vector<bool> &selection;
		ObjectArray& objectArray;
        const float &deltaTime;
        ImGuiIO &io;
        bf::Cursor &cursor;
        bf::MultiCursor &multiCursor;
        bf::Transform &multiTransform;
        glm::vec3 &multiCentre;

        GlfwStruct(bf::Settings &settings1, bf::Camera &camera1, ObjectArray& objectArray1,
                   const float &deltaTime1, ImGuiIO &io1, bf::Cursor &cursor1, bf::MultiCursor &multiCursor1,
                   bf::Transform &multiTransform1, glm::vec3 &multiCentre1);
    };
}

std::string readWholeFile(const char* path);

std::string toString(const glm::vec3& v);
std::string toString(const glm::vec4& v);
bool isnan(const glm::vec3& v);
bool isnan(const glm::vec4& v);

bool almostEqual(float a1, float a2);

#endif //MG1_ZAD2_UTIL_H
