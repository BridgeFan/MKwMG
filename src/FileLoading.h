//
// Created by kamil-hp on 22.04.23.
//

#ifndef MG1_ZAD2_FILELOADING_H
#define MG1_ZAD2_FILELOADING_H
#include <string>

namespace bf {
	class ObjectArray;
	bool loadFromFile(const std::string &path, bf::ObjectArray &objectArray);
	bool saveToFile(const std::string &path, const bf::ObjectArray &objectArray);
}
#endif //MG1_ZAD2_FILELOADING_H
