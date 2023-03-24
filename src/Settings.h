//
// Created by kamil-hp on 03.03.2022.
//

#ifndef MG1_ZAD1_SETTINGS_H
#define MG1_ZAD1_SETTINGS_H
#include <glm/glm.hpp>

enum MouseState {
	LeftClick,
	MiddleClick,
	RightClick,
	None
};

struct Settings {
	glm::mat4 View = glm::mat4(1.0f);
    glm::mat4 InverseView = glm::mat4(1.0f);
    glm::mat4 Projection = glm::mat4(1.0f);
    glm::mat4 InverseProjection = glm::mat4(1.0f);
	float aspect = 1.f;
	MouseState state = None;
	bool isMultiState = false;
	bool isCtrlPressed = false;
	bool isAltPressed = false;
	bool isShiftPressed = false;
    bool isUniformScaling = true;
	uint8_t isAxesLocked = 0x0; //-----ZYX
	int activeIndex = -1;
};

#endif //MG1_ZAD1_SETTINGS_H
