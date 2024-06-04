//
// Created by kamil-hp on 28.08.23.
//

#ifndef MKWMG_INTERSECTIONOBJECT_H
#define MKWMG_INTERSECTIONOBJECT_H
#include "Solids/Solid.h"
#include <vector>

namespace bf {
	class IntersectionObject: public bf::Solid {
		bool isInitPhase=true;
		std::pair<bf::Object*, int> convertToCurve();
	public:
		std::vector<glm::vec4> intersectionPoints;
		bool isLooped=false;
		static bf::ObjectArray* objectArray;
		static void initObjectArray(bf::ObjectArray& objArray) {objectArray=&objArray;}
		bf::Object *obj1, *obj2;
		IntersectionObject();
		//TODO - GUI,
		void ObjectGui() override;
		~IntersectionObject() override;
		void onMergePoints(int p1, int p2) override;
		void draw(const ShaderArray &shader) const override;
		ShaderType getShaderType() const override;
		void updatePoints();
	};
}


#endif//MKWMG_INTERSECTIONOBJECT_H
