//
// Created by kamil-hp on 03.03.2022.
//

#ifndef MG1_ZAD1_SETTINGS_H
#define MG1_ZAD1_SETTINGS_H
#include <glm/glm.hpp>

namespace bf {
    enum MouseState {
        LeftClick,
        MiddleClick,
        RightClick,
        None
    };

    struct Settings {
        float aspect = 1.f;
        MouseState state = None;
        bool isCtrlPressed = false;
        bool isAltPressed = false;
        bool isShiftPressed = false;
        bool isUniformScaling = true;
        uint8_t isAxesLocked = 0x0; //-----ZYX
    };
}

#endif //MG1_ZAD1_SETTINGS_H
