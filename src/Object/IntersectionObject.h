//
// Created by kamil-hp on 28.08.23.
//

#ifndef MKWMG_INTERSECTIONOBJECT_H
#define MKWMG_INTERSECTIONOBJECT_H
#include "ObjectArrayListener.h"
#include "Solids/Solid.h"
#include <vector>

namespace bf {
	class MultiCursor;
	class IntersectionObject: public bf::Solid, public bf::ObjectArrayListener {
		bool isInitPhase=true;
		std::pair<bf::Object*, int> convertToCurve();
		void findIntersection(bool isCursor, double precision, bool isImprovedNeeded=false);
		bool toRemove=false;
		void fillHole(const bf::vec4d& a, const bf::vec4d& b);
	public:
		bool isShown=true;
		void recalculate(bool isTextureToSet=false);
		std::vector<bf::vec4d> intersectionPoints;
		bool isLooped=false;
		bf::Object *obj1, *obj2;
		IntersectionObject(bf::ObjectArray &array);
		IntersectionObject(bf::ObjectArray &array, bf::Object& o1, bf::Object& o2, const glm::vec3& cursor, float precision=0.01f, bool isImprovedNeeded=false);
		IntersectionObject(bf::ObjectArray &array, bf::Object& o1, bf::Object& o2, float precision=0.01f);
		void ObjectGui() override;
		void mergeFlatIntersections(bf::IntersectionObject& i2);
		~IntersectionObject() override;
		void draw(const ShaderArray &shader) const override;
		ShaderType getShaderType() const override;
		bool postInit() override;
		static double distance(const bf::Object& o1, const bf::vec3d& a, const bf::vec2d& t);
		bool isMovable() const override;
		bool isIntersectable() const override;
		[[nodiscard]] virtual bool shouldBeRemoved() const override {return toRemove;}
		void onMergePoints(int p1, int p2) override;
		void onRemoveObject(unsigned int index) override;
		void onMoveObject(unsigned int index) override;
		static double distance(const bf::Object& o1, const bf::Object& o2, const bf::vec4d& t);
		static double movDist(const bf::vec3d &P, const bf::vec3d &Q, const bf::vec3d &P0, const bf::vec3d &t, double d);
	};
}


#endif//MKWMG_INTERSECTIONOBJECT_H
