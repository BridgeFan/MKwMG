//
// Created by kamil-hp on 23.03.2022.
//

#ifndef MG1_ZAD2_UTIL_H
#define MG1_ZAD2_UTIL_H
#include <string>
#include <glm/glm.hpp>
#include <vector>
#include <memory>
struct Settings;
class Object;
class Camera;
class ImGuiIO;
class Cursor;
class MultiCursor;
class Transform;

struct GlfwStruct{
    Settings& settings;
    Camera& camera;
    std::vector<bool>& selection;
    std::vector<std::unique_ptr<Object> >& objects;
    const float& deltaTime;
    ImGuiIO& io;
    Cursor& cursor;
    MultiCursor& multiCursor;
    Transform& multiTransform;
    glm::vec3& multiCentre;

    GlfwStruct(Settings &settings, Camera &camera, std::vector<bool> &selection, std::vector<std::unique_ptr<Object> >& objects,
               const float &deltaTime, ImGuiIO &io, Cursor &cursor, MultiCursor &multiCursor, Transform &multiTransform, glm::vec3 &multiCentre);
};

std::string readWholeFile(const char* path);

std::string toString(const glm::vec3& v);
std::string toString(const glm::vec4& v);
void clearSelection(std::vector<bool>& vec, int index, Settings& settings);
glm::vec3 getCentre(std::vector<bool>& selection, const std::vector<std::unique_ptr<Object> >& objects);
bool isnan(const glm::vec3& v);
bool isnan(const glm::vec4& v);

void deleteObjects(std::vector<bool>& selection, std::vector<std::unique_ptr<Object> >& objects);

#endif //MG1_ZAD2_UTIL_H
