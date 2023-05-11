#pragma once
//
// Created by kamil-hp on 27.03.23.
//

#ifndef MG1_ZAD2_BEZIERCURVE0_H
#define MG1_ZAD2_BEZIERCURVE0_H

#include "BezierCommon.h"

namespace bf {
	class Scene;
	struct ConfigState;
	class ObjectArray;
	class BezierCurve0: public bf::BezierCommon {
	private:
		static int _index;
		void bezierOnAdd() override;
		void bezierOnRemove(unsigned int index) override;
		void bezierOnSwap(unsigned int index1, unsigned int index2) override;
		void bezierOnMove(unsigned int index) override;
	public:
		explicit BezierCurve0(bf::ObjectArray& array);
		BezierCurve0(bf::ObjectArray& array, const std::string& bName);
	};
}


#endif //MG1_ZAD2_BEZIERCURVE0_H
