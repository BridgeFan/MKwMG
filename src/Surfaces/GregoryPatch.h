//
// Created by kamil-hp on 28.05.23.
//

#ifndef MKWMG_GREGORYPATCH_H
#define MKWMG_GREGORYPATCH_H
#include "Object/ObjectArrayListener.h"
#include "Solids/Solid.h"

namespace bf {
	class BezierSurfaceSegment;
	class GregoryPatch: bf::Solid, bf::ObjectArrayListener {
		bf::BezierSurfaceSegment *seg1, *seg2, *seg3;
	public:
		GregoryPatch(bf::ObjectArray& objectArray);
		void onMergePoints(int p1, int p2) override;
		void onRemoveObject(unsigned int index) override;
		void onMoveObject(unsigned int index) override;
	};

}// namespace bf

#endif//MKWMG_GREGORYPATCH_H
