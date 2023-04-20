//
// Created by kamil-hp on 27.03.23.
//

#ifndef MG1_ZAD2_BEZIERCURVEINTER_H
#define MG1_ZAD2_BEZIERCURVEINTER_H

#include "BezierCommon.h"

class GLFWwindow;
namespace bf {
	class Scene;
	struct Settings;
	class ObjectArray;
	class BezierCurveInter: public bf::BezierCommon {
	private:
		unsigned lVAO=UINT_MAX, lVBO=UINT_MAX, lIBO=UINT_MAX;
		static int _index;
		std::vector<glm::vec3> positions;
		void bezierOnAdd() override;
		void bezierOnRemove(unsigned int index) override;
		void bezierOnSwap(unsigned int index1, unsigned int index2) override;
		void bezierOnMove(unsigned int index) override;
		void recalculate(bool wasSizeChanged);
    public:
		~BezierCurveInter();
        void draw(const Shader &shader) const override;

    public:
		explicit BezierCurveInter(bf::ObjectArray& array);
	};
}


#endif //MG1_ZAD2_BEZIERCURVEINTER_H
