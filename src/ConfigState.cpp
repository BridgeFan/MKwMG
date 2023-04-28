//
// Created by kamil-hp on 28.04.23.
//
#include "ConfigState.h"
#include "Util.h"
#include <jsoncpp/json/json.h>
#include <fstream>

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
    screenWidth = value.get("screen_width",1024).asInt();
    screenHeight = value.get("screen_height",768).asInt();
    isUniformScaling = value.get("is_uniform_scaling",true).asBool();
    isAxesLocked = value.get("is_axes_locked",0x0).asUInt();
    divisionNum = value.get("division_num",8).asInt();
    totalDivision = value.get("total_division",4096).asInt();
    maxTorusFragments = value.get("max_torus_fragments",60).asInt();
    movementSpeed = value.get("movement_speed",2.5f).asFloat();
    rotationSpeed = value.get("rotation_speed",20.f).asFloat();
    cameraZoom = value.get("camera_zoom",45.f).asFloat();
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
    value["camera_zoom"]=cameraZoom;
    std::ofstream file(configPath);
    if(!file.good())
        return;
    file << value;
}


