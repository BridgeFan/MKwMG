//
// Created by kamil-hp on 23.03.2022.
//
#include <memory>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iostream>
#include "Util.h"
#include "ConfigState.h"
#include "Scene.h"
#ifdef USE_STD_FORMAT
#include <format>
#endif

std::string toString(const glm::vec3& v) {
	return "("+std::to_string(v.x)+","+std::to_string(v.y)+", "+std::to_string(v.z)+")";
}

std::string toString(const glm::vec4& v) {
	return "("+std::to_string(v.x)+","+std::to_string(v.y)+", "+std::to_string(v.z)+","+std::to_string(v.w)+")";
}

std::string readWholeFile(const std::string& path) {
    std::ifstream t(path);
    if(t.bad()) {
#ifdef USE_STD_FORMAT
        std::cerr << std::format("{} not found!\n", path);
#else
        std::cerr << path << "not found!\n";
#endif
        return "";
    }
    std::stringstream buffer;
    buffer << t.rdbuf();
    return buffer.str();
}

bool isnan(const glm::vec3 &v) {
    return std::isnan(v.x) || std::isnan(v.y) || std::isnan(v.z);
}
bool isnan(const glm::vec4 &v) {
    return std::isnan(v.x) || std::isnan(v.y) || std::isnan(v.z) || std::isnan(v.w);
}

bool almostEqual(float a1, float a2, float eps) {
	return std::abs(a1-a2)<eps*std::max(std::max(std::abs(a1),std::abs(a2)),1e-5f);
}

glm::vec3 bf::toScreenPos(int screenWidth, int screenHeight, const glm::vec3& worldPos, const glm::mat4& view, const glm::mat4& projection) {
    auto v = projection*view*glm::vec4(worldPos,1.f);
    v/=v.w;
    v.x=(v.x+1.f)*static_cast<float>(screenWidth)*.5f;
    v.y=(1.f-v.y)*static_cast<float>(screenHeight)*.5f;
    return {v.x,v.y,v.z};
}

glm::vec3 bf::toGlobalPos(int screenWidth, int screenHeight, const glm::vec3& mousePos, const glm::mat4& inverseView, const glm::mat4& inverseProjection) {
    auto mp = mousePos;
    mp.x = 2.f*mp.x/static_cast<float>(screenWidth)-1.f;
    mp.y = 1.f-2.f*mp.y/static_cast<float>(screenHeight);
    auto v = inverseView*inverseProjection*glm::vec4(mp,1.f);
    v/=v.w;
    return {v.x,v.y,v.z};
}


bool bf::isInBounds(int screenWidth, int screenHeight, const glm::vec2 &screenPos) {
    return isInBounds(screenWidth,screenHeight,{screenPos.x,screenPos.y,0.f});
}

bool bf::isInBounds(int screenWidth, int screenHeight, const glm::vec3 &mousePos) {
    if(std::abs(mousePos.z)>1.f)
        return false;
    if(mousePos.x<0 || mousePos.x>screenWidth)
        return false;
    if(mousePos.y<0 || mousePos.y>screenHeight)
        return false;
    return true;
}

float bf::getDeltaTime() {
    typedef std::chrono::high_resolution_clock clock;
    typedef std::chrono::duration<float> duration;
    static clock::time_point start = clock::now();
    duration elapsed = clock::now() - start;
    start = clock::now();
    return elapsed.count();
}
