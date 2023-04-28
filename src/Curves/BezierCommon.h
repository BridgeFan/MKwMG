//
// Created by kamil-hp on 19.04.23.
//

#ifndef MG1_ZAD2_BEZIERCOMMON_H
#define MG1_ZAD2_BEZIERCOMMON_H

#include "Solids/Solid.h"
#include "Object/ObjectArrayListener.h"
#include "BasicBezier.h"

namespace bf {
	class BezierCommon: public bf::Solid, public bf::ObjectArrayListener {
	private:
		static int _index;
	protected:
        bool isTmpLineDrawn=false;
		bool isTmpPointDrawn=false;
		bf::BasicBezier bezier;
		bool isPolygonVisible, isCurveVisible, isLineDrawn;
		std::size_t activeIndex;
		std::vector<unsigned> pointIndices;
		void recalculate();
		void recalculatePart(int index); //index of part
		virtual void bezierOnAdd() = 0;
		virtual void bezierOnRemove(unsigned index) = 0;
		virtual void bezierOnSwap(unsigned index1, unsigned index2) = 0;
		virtual void bezierOnMove(unsigned index) = 0;
        glm::vec3 getPoint(int pIndex) const;
	public:
		explicit BezierCommon(bf::ObjectArray& array);
		void postInit() override;
		void onRemoveObject(unsigned index) override;
		void onMoveObject(unsigned index) override;

		[[nodiscard]] std::vector<unsigned int> usedVectors() const override;

		bool addPoint(unsigned index) override;
		bool removePoint(unsigned index);
		virtual void draw(const ShaderArray &shaderArray) const override;
		void ObjectGui() override;
		[[nodiscard]] bool isMovable() const override {return false;}

		bool onKeyPressed(bf::event::Key key, bf::event::ModifierKeyBit mods) override;

        ShaderType getShaderType() const override;

    };
}


#endif //MG1_ZAD2_BEZIERCOMMON_H
