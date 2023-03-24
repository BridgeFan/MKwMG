//
// Created by kamil-hp on 21.03.2022.
//

#ifndef MG1_ZAD2_CURSOR_H
#define MG1_ZAD2_CURSOR_H
#include "Object.h"
#include "Solid.h"

struct Settings;
struct GLFWwindow;
class Cursor {
private:
	Solid lines[3];
    void initLines();
public:
	Transform transform;
	explicit Cursor(const Transform& transform=Transform::Default);
	void draw(const Shader& shader, const Settings& settings);
	void ObjectGui(GLFWwindow* window, const glm::mat4& view, const glm::mat4& inverseView, const glm::mat4& projection, const glm::mat4& inverseProjection);
};


#endif //MG1_ZAD2_CURSOR_H
