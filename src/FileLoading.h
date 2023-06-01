#pragma once
//
// Created by kamil-hp on 22.04.23.
//

#ifndef MG1_ZAD2_FILELOADING_H
#define MG1_ZAD2_FILELOADING_H
#include <string>
#include <iosfwd>

namespace bf {
	class Camera;
	class ObjectArray;
	bool loadFromFile(bf::ObjectArray &objectArray, const std::string &path = "saves/save.json");
	bool saveToFile(const bf::ObjectArray &objectArray, const std::string &path = "saves/save.json");
    bool loadFromStream(bf::ObjectArray &objectArray, std::istream& in);
    bool saveToStream(const bf::ObjectArray &objectArray, std::ostream& out);
}
#endif //MG1_ZAD2_FILELOADING_H
