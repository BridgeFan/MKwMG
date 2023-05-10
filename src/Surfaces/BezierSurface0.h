//
// Created by kamil-hp on 06.05.23.
//

#ifndef MG1_ZAD2_BEZIERSURFACE0_H
#define MG1_ZAD2_BEZIERSURFACE0_H

#include "Object/Object.h"
#include "Object/ObjectArrayListener.h"
#include <string>
#include <array>
#include <vector>
#include "BezierSurfaceSegment0.h"

namespace bf {
	class ObjectArray;
    class Cursor;

	class BezierSurface0: public bf::Object, public bf::ObjectArrayListener {
		friend bool loadFromFile(bf::ObjectArray &objectArray, const std::string &path);
		static int _index;
        const bf::Cursor& cursor;
		bool isWrappedX=false, isWrappedY=false;
        bool isPolygonVisible=false, isSurfaceVisible=true;
	public:
        ~BezierSurface0() override;
        glm::vec<2,int> samples;
		std::vector<bf::BezierSurfaceSegment0> segments;
		void postInit() override;
		void draw(const ShaderArray &shader) const override;
		void ObjectGui() override;
		bool isMovable() const override;
		ShaderType getShaderType() const override;
		explicit BezierSurface0(bf::ObjectArray &objectArray, const std::string &objName, const bf::Cursor& c);
		explicit BezierSurface0(bf::ObjectArray &objectArray, const bf::Cursor& c);
		void onRemoveObject(unsigned int index) override;
		void onMoveObject(unsigned int index) override;
	};
}


#endif //MG1_ZAD2_BEZIERSURFACE0_H
