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
		int activeBezierIndex=-1;
		static int _index;
		void bezierOnAdd() override;
		void bezierOnRemove(unsigned int index) override;
		void bezierOnSwap(unsigned int index1, unsigned int index2) override;
		void bezierOnMove(unsigned int index) override;
		void recalculate();
	public:
		explicit BezierCurve2(bf::ObjectArray& array);
		bool onMouseButtonPressed(int button, int mods) override;
		bool onMouseButtonReleased(int button, int mods) override;
		void onMouseMove(const glm::vec2 &oldPos, const glm::vec2 &newPos) override;
	};
}


#endif //MG1_ZAD2_BEZIERCURVE2_H
