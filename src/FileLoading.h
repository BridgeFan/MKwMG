#pragma once
//
// Created by kamil-hp on 22.04.23.
//

#ifndef MG1_ZAD2_FILELOADING_H
#define MG1_ZAD2_FILELOADING_H
#include <string>

namespace bf {
	class Camera;
	class ObjectArray;
	bool loadFromFile(bf::ObjectArray &objectArray, bf::Camera& camera, const std::string &path = "saves/save.json");
	bool saveToFile(const bf::ObjectArray &objectArray, bf::Camera& camera, const std::string &path = "saves/save.json");
}
#endif //MG1_ZAD2_FILELOADING_H
