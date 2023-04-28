//
// Created by kamil-hp on 03.03.2022.
//

#ifndef MG1_ZAD1_CONFIGSTATE_H
#define MG1_ZAD1_CONFIGSTATE_H

#include <cstdint>

namespace bf {
    enum MouseState {
        LeftClick,
        MiddleClick,
        RightClick,
        None
    };

    struct ConfigState {
        ConfigState();
        ~ConfigState();
        //state of program
        MouseState state = None;
        bool isCtrlPressed = false;
        bool isAltPressed = false;
        bool isShiftPressed = false;
        float mouseX = 0.f;
        float mouseY = 0.f;
        //state set from UI / config
        int screenWidth = 1024;
        int screenHeight = 768;
        bool isUniformScaling = true;
        uint8_t isAxesLocked = 0x0; //-----ZYX
        int divisionNum = 8;
        int totalDivision = 4096;
        int maxTorusFragments = 60;
        float movementSpeed = 2.5f;
        float rotationSpeed = 20.f;
        float cameraZoom = 45.f;
    };
}

#endif //MG1_ZAD1_CONFIGSTATE_H
