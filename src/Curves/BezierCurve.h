//
// Created by kamil-hp on 27.03.23.
//

#ifndef MG1_ZAD2_BEZIERCURVE_H
#define MG1_ZAD2_BEZIERCURVE_H

#include "BezierCommon.h"

class GLFWwindow;
namespace bf {
	class Scene;
	struct Settings;
	class ObjectArray;
	class BezierCurve: public bf::BezierCommon {
	private:
		static int _index;
		void bezierOnAdd() override;
		void bezierOnRemove(unsigned int index) override;
		void bezierOnSwap(unsigned int index1, unsigned int index2) override;
		void bezierOnMove(unsigned int index) override;
	public:
		explicit BezierCurve(bf::ObjectArray& array);
	};
}


#endif //MG1_ZAD2_BEZIERCURVE_H
