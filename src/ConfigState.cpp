//
// Created by kamil-hp on 28.04.23.
//
#include "ConfigState.h"
#include "Util.h"
#include <jsoncpp/json/json.h>
#include <fstream>
#include "Event.h"

constexpr const char* configPath = "../config.json";

bf::ConfigState::ConfigState() {
    Json::Value value;
    std::ifstream file(configPath);
    if(!file.good())
        return;
    try {
        file >> value;
    }
    catch(...) {
        return;
    }
    file.close();
    screenWidth = value.get("screen_width",screenWidth).asInt();
    screenHeight = value.get("screen_height",screenHeight).asInt();
    isUniformScaling = value.get("is_uniform_scaling",isUniformScaling).asBool();
    isAxesLocked = value.get("is_axes_locked",isAxesLocked).asUInt();
    divisionNum = value.get("division_num",divisionNum).asInt();
    totalDivision = value.get("total_division",totalDivision).asInt();
    maxTorusFragments = value.get("max_torus_fragments",maxTorusFragments).asInt();
    movementSpeed = value.get("movement_speed",movementSpeed).asFloat();
    rotationSpeed = value.get("rotation_speed",rotationSpeed).asFloat();
	pointRadius = value.get("point_radius",pointRadius).asFloat();
	backgroundColorR = value.get("background_color_r",backgroundColorR).asUInt();
	backgroundColorG = value.get("background_color_g",backgroundColorG).asUInt();
	backgroundColorB = value.get("background_color_b",backgroundColorB).asUInt();
	if(value.isMember("camera") && value["camera"].isObject()) {
		Json::Value cameraValue = value["camera"];
		cameraFOV = cameraValue.get("fov",cameraFOV).asFloat();
		cameraFOVmin = cameraValue.get("fov_min",cameraFOVmin).asFloat();
		cameraFOVmax = cameraValue.get("fov_max",cameraFOVmax).asFloat();
		cameraNear = cameraValue.get("near",cameraNear).asFloat();
		cameraFar = cameraValue.get("far",cameraFar).asFloat();
		cameraInitPos.x = cameraValue.get("pos_x",cameraInitPos.x).asFloat();
		cameraInitPos.y = cameraValue.get("pos_y",cameraInitPos.y).asFloat();
		cameraInitPos.z = cameraValue.get("pos_z",cameraInitPos.z).asFloat();
		cameraInitRot.x = cameraValue.get("rot_x",cameraInitRot.x).asFloat();
		cameraInitRot.y = cameraValue.get("rot_y",cameraInitRot.y).asFloat();
		cameraInitRot.z = cameraValue.get("rot_z",cameraInitRot.z).asFloat();
	}
}

bf::ConfigState::~ConfigState() {
    Json::Value value;
    value["screen_width"]=screenWidth;
    value["screen_height"]=screenHeight;
    value["is_uniform_scaling"]=isUniformScaling;
    value["is_axes_locked"]=isAxesLocked;
    value["division_num"]=divisionNum;
    value["total_division"]=totalDivision;
    value["max_torus_fragments"]=maxTorusFragments;
    value["movement_speed"]=movementSpeed;
    value["rotation_speed"]=rotationSpeed;
	value["point_radius"]=pointRadius;
	value["background_color_r"]=backgroundColorR;
	value["background_color_g"]=backgroundColorG;
	value["background_color_b"]=backgroundColorB;
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

float bf::ConfigState::getCameraNear() const {
	return cameraNear;
}

float bf::ConfigState::getCameraFar() const {
	return cameraFar;
}

const glm::vec3 &bf::ConfigState::getCameraInitPos() const {
	return cameraInitPos;
}

const glm::vec3 &bf::ConfigState::getCameraInitRot() const {
	return cameraInitRot;
}
