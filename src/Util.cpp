//
// Created by kamil-hp on 23.03.2022.
//
#include "Util.h"
#include "ConfigState.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>

std::string toString(const glm::vec3& v) {
	return "("+std::to_string(v.x)+","+std::to_string(v.y)+", "+std::to_string(v.z)+")";
}

std::string toString(const glm::vec4& v) {
	return "("+std::to_string(v.x)+","+std::to_string(v.y)+", "+std::to_string(v.z)+","+std::to_string(v.w)+")";
}

std::string readWholeFile(const std::string& path) {
    std::ifstream t(path);
    if(t.bad()) {
        std::cerr << std::format("{} not found!\n", path);
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
	return std::abs(a1-a2)<=eps*std::max(std::max(std::abs(a1),std::abs(a2)),1e-5f);
}
bool almostEqual(const glm::vec3& v1, const glm::vec3& v2, float eps) {
	return bf::sqrDistance(v1,v2)<=eps;
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
    using clock = std::chrono::high_resolution_clock;
    using duration = std::chrono::duration<float>;
    static clock::time_point start = clock::now();
    duration elapsed = clock::now() - start;
    start = clock::now();
    return elapsed.count();
}
float bf::sqrLength(const glm::vec3 &a) {
	return a.x*a.x+a.y*a.y+a.z*a.z;
}
float bf::length(const glm::vec3 &a) {
	return std::sqrt(sqrLength(a));
}
float bf::distance(const glm::vec3 &a, const glm::vec3 &b) {
	return length(a-b);
}
float bf::sqrDistance(const glm::vec3 &a, const glm::vec3 &b) {
	return sqrLength(a - b);
}
float bf::degrees(float a) {
	return 180.f/PI*a;
}
glm::vec3 bf::degrees(const glm::vec3& a) {
	return (180.f/PI)*a;
}
float bf::radians(float a) {
	return a*PI/180.f;
}
glm::vec3 bf::radians(const glm::vec3& a) {
	return a*(PI/180.f);
}
glm::vec3 bf::matrixToEulerXYZ(const glm::mat4 &M) {
	//calculate X
	float x = std::atan2(M[2][1], M[2][2]);
	//calculate Y
	float cy = std::sqrt(M[0][0]*M[0][0] + M[1][0]*M[1][0]);
	float y = std::atan2(-M[2][0], cy);
	//calculate Z
	float sx = std::sin(x);
	float cx = std::cos(x);
	float z = std::atan2(sx*M[0][2] - cx*M[0][1], cx*M[1][1] - sx*M[1][2]);
	return {-x,-y,-z};
}

glm::vec3 bf::matrixToEulerYXZ(const glm::mat4 &M) {
	//calculate Y
	float x = std::atan2(M[2][0], M[2][2]);
	//calculate X
	float cy = std::sqrt(M[0][1]*M[0][1] + M[1][1]*M[1][1]);
	float y = std::atan2(-M[2][1], cy);
	//calculate Z
	float sx = std::sin(x);
	float cx = std::cos(x);
	float z = std::atan2(sx*M[1][2] - cx*M[1][0], cx*M[0][0] - sx*M[0][2]);
	return {-x,-y,-z};
}