//
// Created by kamil-hp on 28.04.23.
//
#include "ConfigState.h"
#include "Util.h"
#include <fstream>
#include <iostream>
#include "Event.h"
#include "JsonUtil.h"

constexpr const char* configPath = "config.cfg";

bf::ConfigState::ConfigState() {
    std::cout << "Config loading begin\n";
    Json::Value value;
    if(!loadFromFile(value, configPath)) {
        std::cout << "Config file not found\n";
        return;
    }
    std::cout << "Config file found\n";
	load(value, screenWidth, "screen_width");
	load(value, screenHeight, "screen_height");
    load(value, screenFullscreen, "screen_fullscreen");
	load(value, isUniformScaling, "is_uniform_scaling");
	load(value, isAxesLocked, "is_axes_locked");
	load(value, divisionNum, "division_num");
	load(value, totalDivision, "total_division");
	load(value, maxTorusFragments, "max_torus_fragments");
	load(value, movementSpeed, "movement_speed");
	load(value, rotationSpeed, "rotation_speed");
	load(value, pointRadius, "point_radius");
	load(value, stereoscopic, "stereoscopic");
	load(value, pointRadius, "point_radius");
    load(value, grayPercentage, "gray_percentage");
	load(value, backgroundColorR, "background_color_r");
	load(value, backgroundColorG, "background_color_g");
	load(value, backgroundColorB, "background_color_b");
    load(value, convergence, "convergence");
    load(value, IOD, "iod");
    load(value, fontPath, "font_path");
	if(value.isMember("camera") && value["camera"].isObject()) {
		Json::Value cameraValue = value["camera"];
		load(cameraValue, cameraFOV, "fov");
		load(cameraValue, cameraFOVmin, "fov_min");
		load(cameraValue, cameraFOVmax, "fov_max");
		load(cameraValue, cameraNear, "near");
		load(cameraValue, cameraFar, "far");
		load(cameraValue, cameraInitPos.x, "pos_x");
		load(cameraValue, cameraInitPos.y, "pos_y");
		load(cameraValue, cameraInitPos.z, "pos_z");
		load(cameraValue, cameraInitRot.x, "rot_x");
		load(cameraValue, cameraInitRot.y, "rot_y");
		load(cameraValue, cameraInitRot.z, "rot_z");
	}
    std::cout << "Config loading finished\n";
}

bf::ConfigState::~ConfigState() {
    Json::Value value;
    value["screen_width"]=screenWidth;
    value["screen_height"]=screenHeight;
    value["screen_fullscreen"]=screenFullscreen;
    value["is_uniform_scaling"]=isUniformScaling;
    value["is_axes_locked"]=isAxesLocked;
    value["division_num"]=divisionNum;
    value["total_division"]=totalDivision;
    value["max_torus_fragments"]=maxTorusFragments;
    value["movement_speed"]=movementSpeed;
    value["rotation_speed"]=rotationSpeed;
	value["point_radius"]=pointRadius;
    value["gray_percentage"]=grayPercentage;
	value["stereoscopic"]=stereoscopic;
	value["background_color_r"]=backgroundColorR;
	value["background_color_g"]=backgroundColorG;
	value["background_color_b"]=backgroundColorB;
    value["convergence"]=convergence;
    value["iod"]=IOD;
    value["font_path"]=fontPath;
	//camera
	Json::Value cameraValue;
	cameraValue["fov"]=cameraFOV;
	cameraValue["fov_max"]=cameraFOVmax;
	cameraValue["fov_min"]=cameraFOVmin;
	cameraValue["near"]=cameraNear;
	cameraValue["far"]=cameraFar;
	cameraValue["pos_x"]=cameraInitPos.x;
	cameraValue["pos_y"]=cameraInitPos.y;
	cameraValue["pos_z"]=cameraInitPos.z;
	cameraValue["rot_x"]=cameraInitRot.x;
	cameraValue["rot_y"]=cameraInitRot.y;
	cameraValue["rot_z"]=cameraInitRot.z;
    cameraValue["font_path"]=fontPath;
	value["camera"]=cameraValue;
    std::ofstream file(configPath);
    if(!file.good())
        return;
    file << value;
}

void bf::ConfigState::onKeyPressed(bf::event::Key key, bf::event::ModifierKeyBit) {
    using namespace bf::event;
    if(key==Key::LeftControl || key==Key::RightControl)
        isCtrlPressed = true;
    if(key==Key::LeftAlt || key==Key::RightAlt || key==Key::A) //A is additional ALT
        isAltPressed = true;
    if(key==Key::LeftShift || key==Key::RightShift)
        isShiftPressed = true;
    switch(key) {
        case Key::X:
            if(isAxesLocked%2)
                isAxesLocked -= 0x1;
            else
                isAxesLocked += 0x1;
            break;
        case Key::Y:
            if((isAxesLocked>>1)%2)
                isAxesLocked -= 0x2;
            else
                isAxesLocked += 0x2;
            break;
        case Key::Z:
            if((isAxesLocked>>2)%2)
                isAxesLocked -= 0x4;
            else
                isAxesLocked += 0x4;
            break;
        case Key::U:
            isUniformScaling = !isUniformScaling;
            break;
        case Key::S:
            stereoscopic = !stereoscopic;
            break;
        case Key::B:
            if(!isBoxSelect) {
                boxMouseX = mouseX;
                boxMouseY = mouseY;
            }
            isBoxSelect = !isBoxSelect;
            break;
        default:
            ;
    }
}
void bf::ConfigState::onKeyReleased(bf::event::Key key, bf::event::ModifierKeyBit) {
    using namespace bf::event;
    if(key==Key::LeftControl || key==Key::RightControl)
        isCtrlPressed = false;
    if(key==Key::LeftAlt || key==Key::RightAlt || key==Key::A) //A is additional ALT
        isAltPressed = false;
    if(key==Key::LeftShift || key==Key::RightShift)
        isShiftPressed = false;
}

void bf::ConfigState::onMouseButtonPressed(bf::event::MouseButton button, bf::event::ModifierKeyBit) {
    using namespace bf::event;
    if(button==MouseButton::Right)
        state = bf::RightClick;
    else if(button==MouseButton::Middle)
        state = bf::MiddleClick;
}


void bf::ConfigState::onMouseButtonReleased(bf::event::MouseButton button, bf::event::ModifierKeyBit) {
    using namespace bf::event;
    if((button == MouseButton::Right && state == bf::RightClick) || (button == MouseButton::Middle && state == bf::MiddleClick))
        state = bf::None;
}

float bf::ConfigState::getCameraFoVmax() const {
	return cameraFOVmax;
}

float bf::ConfigState::getCameraFoVmin() const {
	return cameraFOVmin;
}

const glm::vec3 &bf::ConfigState::getCameraInitPos() const {
	return cameraInitPos;
}

const glm::vec3 &bf::ConfigState::getCameraInitRot() const {
	return cameraInitRot;
}

const std::string &bf::ConfigState::getFontPath() const {
    return fontPath;
}
