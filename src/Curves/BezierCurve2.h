//
// Created by kamil-hp on 27.03.23.
//

#ifndef MG1_ZAD2_BEZIERCURVE2_H
#define MG1_ZAD2_BEZIERCURVE2_H

#include "BezierCommon.h"

class GLFWwindow;
namespace bf {
	class Scene;
	struct Settings;
	class BezierCurve2: public bf::BezierCommon {
	private:
		static int _index;
		void bezierOnAdd() override;
		void bezierOnRemove(unsigned int index) override;
		void bezierOnSwap(unsigned int index1, unsigned int index2) override;
		void bezierOnMove(unsigned int index) override;
		glm::vec3 getPoint(int pIndex) const;
		void recalculate(bool wasResized);
	public:
		explicit BezierCurve2(bf::ObjectArray& array);
    };
}


#endif //MG1_ZAD2_BEZIERCURVE2_H
