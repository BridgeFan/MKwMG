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
    class Object;
    class Camera;
    class Cursor;
    class MultiCursor;
    class Transform;
    struct GlfwStruct {
        bf::Settings &settings;
        bf::Camera &camera;
        std::vector<bool> &selection;
        std::vector<std::unique_ptr<bf::Object> > &objects;
        const float &deltaTime;
        ImGuiIO &io;
        bf::Cursor &cursor;
        bf::MultiCursor &multiCursor;
        bf::Transform &multiTransform;
        glm::vec3 &multiCentre;

        GlfwStruct(bf::Settings &settings1, bf::Camera &camera1, std::vector<bool> &selection1,
                   std::vector<std::unique_ptr<bf::Object> > &objects1,
                   const float &deltaTime1, ImGuiIO &io1, bf::Cursor &cursor1, bf::MultiCursor &multiCursor1,
                   bf::Transform &multiTransform1, glm::vec3 &multiCentre1);
    };
    void clearSelection(std::vector<bool>& vec, int index, bf::Settings& settings);
    glm::vec3 getCentre(std::vector<bool>& selection, const std::vector<std::unique_ptr<bf::Object> >& objects);
    void deleteObjects(std::vector<bool>& selection, std::vector<std::unique_ptr<bf::Object> >& objects);
}

std::string readWholeFile(const char* path);

std::string toString(const glm::vec3& v);
std::string toString(const glm::vec4& v);
bool isnan(const glm::vec3& v);
bool isnan(const glm::vec4& v);

bool almostEqual(float a1, float a2);

#endif //MG1_ZAD2_UTIL_H
