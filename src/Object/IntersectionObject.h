//
// Created by kamil-hp on 28.08.23.
//

#ifndef MKWMG_INTERSECTIONOBJECT_H
#define MKWMG_INTERSECTIONOBJECT_H
#include "Solids/Solid.h"
#include <vector>

namespace bf {
	class MultiCursor;
	class IntersectionObject: public bf::Solid {
		bool isInitPhase=true;
		std::pair<bf::Object*, int> convertToCurve();
		void findIntersection(bool isCursor);
		static double distance(const bf::Object& o1, const bf::Object& o2, const bf::vec4d& t);
		bool toRemove=false;
	public:
		std::vector<glm::vec4> intersectionPoints;
		bool isLooped=false;
		static bf::ObjectArray* objectArray;
		static void initObjectArray(bf::ObjectArray& objArray) {objectArray=&objArray;}
		bf::Object *obj1, *obj2;
		IntersectionObject(bf::ObjectArray &);
		//TODO - GUI,
		void ObjectGui() override;
		~IntersectionObject() override;
		void draw(const ShaderArray &shader) const override;
		ShaderType getShaderType() const override;
		bool postInit() override;
		static double distance(const bf::Object& o1, const bf::vec3d& a, const bf::vec2d& t);
		bool isMovable() const override;
		bool isIntersectable() const override;
		[[nodiscard]] virtual bool shouldBeRemoved() const override {return toRemove;}
		void onMergePoints(int p1, int p2) override;
	};
}


#endif//MKWMG_INTERSECTIONOBJECT_H
