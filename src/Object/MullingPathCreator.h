//
// Created by kamil-tp on 12.11.24.
//

#ifndef MULLINGPATHCREATOR_H
#define MULLINGPATHCREATOR_H
#include "Object.h"
#include "Shader/ShaderArray.h"
#include "Solids/Solid.h"

namespace bf {

class MullingPathCreator: public bf::Object {
		bool areIntersectionsFound = false, isZUp=true;
		double scale;
		bf::vec3d move;
		bool shallBeDestroyed = false;
		std::vector<int> mullingRadius = {16, 10, 8, 1}; //second is flat
		bf::vec3d c(const bf::vec3d& p) const; //conversion of point from screen space to mulling space
		bf::vec3d dc(const bf::vec3d& p) const; //conversion of point from mulling space to screen space
		std::vector<bf::DummySolid> dummySolids;
	public:
		bf::ObjectArray& objectArray;
		MullingPathCreator(bf::ObjectArray& a);
		~MullingPathCreator();
		void draw(const bf::ShaderArray &) const override;
		void onMergePoints(int, int) override {}
		[[nodiscard]] bf::ShaderType getShaderType() const override {return MultipleShaders;}
		void ObjectGui() override;
		[[nodiscard]] bool isMovable() const override { return false; }
		bool postInit() override {return shallBeDestroyed;}
		bool saveToFile(const std::string& a, const std::vector<bf::vec3d>& points);
			//true if success
		void createApproximateMullingPath();
};

} // bf

#endif //MULLINGPATHCREATOR_H
