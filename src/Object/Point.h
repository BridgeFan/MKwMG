#pragma once
//
// Created by kamil-hp on 20.03.2022.
//

#ifndef MG1_ZAD2_POINT_H
#define MG1_ZAD2_POINT_H

#include "Object/Object.h"

namespace bf {
	class ObjectArray;
	class Point : public bf::Object {
	private:
		static int index;
		static void Init();
		static unsigned int VBO, VAO;
		static bool isInited;
		static bf::ObjectArray* objectArray;
	public:
		Point(const bf::Transform &transform, const std::string &pointName);
		explicit Point(const std::string &pointName) : Point(bf::Transform::Default, pointName) {}
		explicit Point(const bf::Transform &t = bf::Transform::Default) : Point( t, "Point " + std::to_string(
				index)) { index++; }
		void draw(const bf::ShaderArray &shader) const override;
		static void initObjArrayRef(bf::ObjectArray& objArray);

		void setPosition(const glm::vec3 &pos) override;

		void setTransform(const Transform &t) override;

		void setTransform(Transform &&t) override;

        ShaderType getShaderType() const override;
		void onMergePoints(int, int) override {}
    };
}


#endif //MG1_ZAD2_POINT_H
