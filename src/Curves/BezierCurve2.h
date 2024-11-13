#pragma once
//
// Created by kamil-hp on 27.03.23.
//

#ifndef MG1_ZAD2_BEZIERCURVE2_H
#define MG1_ZAD2_BEZIERCURVE2_H

#include "BezierCommon.h"

namespace bf {
	class Scene;
	struct ConfigState;
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
		explicit BezierCurve2(bf::ObjectArray& array, const std::string& name);
		bool onMouseButtonPressed(bf::event::MouseButton button, bf::event::ModifierKeyBit mods) override;
		bool onMouseButtonReleased(bf::event::MouseButton button, bf::event::ModifierKeyBit mods) override;
		void onMouseMove(const glm::vec2 &oldPos, const glm::vec2 &newPos) override;
	};
}


#endif //MG1_ZAD2_BEZIERCURVE2_H
