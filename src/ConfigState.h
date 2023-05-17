#pragma once
//
// Created by kamil-hp on 03.03.2022.
//

#ifndef MG1_ZAD1_CONFIGSTATE_H
#define MG1_ZAD1_CONFIGSTATE_H

#include <cstdint>
#include <glm/vec3.hpp>
#include <string>

namespace bf {
    namespace event {
        enum class Key: int;
        enum class MouseButton: int;
        enum ModifierKeyBit: int;
    }
    enum MouseState {
        LeftClick,
        MiddleClick,
        RightClick,
        None
    };

    struct ConfigState {
	private:
		float cameraFOVmin = 5.f;
		float cameraFOVmax = 120.f;
		glm::vec3 cameraInitPos = {0.0f, 0.0f, -10.0f};
		glm::vec3 cameraInitRot = {0.0f, 0.0f, 0.0f};
        std::string fontPath;
    public:
        ConfigState();
        ~ConfigState();
        //state of program
        MouseState state = None;
        bool isCtrlPressed = false;
        bool isAltPressed = false;
        bool isShiftPressed = false;
        bool isBoxSelect = false;
        float boxMouseX = 0.f, boxMouseY = 0.f;
        float mouseX = 0.f;
        float mouseY = 0.f;
        float deltaTime = 0.f;
        //state set from UI / config
        int screenWidth = 1024;
        int screenHeight = 768;
        bool screenFullscreen = false;
        bool isUniformScaling = true;
        uint8_t isAxesLocked = 0x0; //-----ZYX
        int divisionNum = 8;
        int totalDivision = 4096;
        int maxTorusFragments = 60;
        float movementSpeed = 2.5f;
        float rotationSpeed = 20.f;
        float cameraFOV = 45.f;
		float pointRadius = 8.f;
		uint8_t backgroundColorR = 64;
		uint8_t backgroundColorG = 64;
		uint8_t backgroundColorB = 51;
        float cameraNear = .1f;
        float cameraFar = 100.f;
        bool stereoscopic = false;
        float grayPercentage = .0f;
		float getCameraFoVmax() const;
		float getCameraFoVmin() const;
		const glm::vec3 &getCameraInitPos() const;
		const glm::vec3 &getCameraInitRot() const;
        const std::string &getFontPath() const;
        void onKeyPressed(bf::event::Key key, bf::event::ModifierKeyBit modKeyBit);
        void onKeyReleased(bf::event::Key key, bf::event::ModifierKeyBit modKeyBit);
        void onMouseButtonPressed(bf::event::MouseButton button, bf::event::ModifierKeyBit mods);
        void onMouseButtonReleased(bf::event::MouseButton button, bf::event::ModifierKeyBit mods);
        float IOD = .2f;
        float convergence = 0.f;
    };
}

#endif //MG1_ZAD1_CONFIGSTATE_H
