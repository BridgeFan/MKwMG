//
// Created by kamil-hp on 20.03.2022.
//

#ifndef MG1_ZAD2_POINT_H
#define MG1_ZAD2_POINT_H

#include "Object.h"

class Point: public Object {
private:
	static int index;
	static void Init();
	static unsigned int VBO, VAO;
	static bool isInited;
public:
	Point(const Transform& transform, const std::string& name);
	explicit Point(const std::string& name): Point(Transform::Default, name) {}
	explicit Point(const Transform& transform=Transform::Default): Point(transform, "Point "+std::to_string(index)) {index++;}
	void draw(const Shader& shader) const override;
};


#endif //MG1_ZAD2_POINT_H
