//
// Created by kamil-tp on 12.11.24.
//

#ifndef MULLINGPATHCREATOR_H
#define MULLINGPATHCREATOR_H
#include "Object.h"
#include "Shader/ShaderArray.h"
#include "Solids/Solid.h"

#include <memory>

namespace bf {
	class IntersectionObject;
	class Point;
    class EquidistanceSurface;

class MullingPathCreator: public bf::Object {
		bool areIntersectionsFound = false, isZUp=true;
		double scale;
		bf::vec3d move;
		bool shallBeDestroyed = false;
		unsigned textureIndex = UINT_MAX;
		std::unique_ptr<bf::Point> point;
		std::unique_ptr<bf::DummySolid> debugSolid;
		std::vector<int> mullingRadius = {16, 10, 8, 1}; //second is flat
		std::map<unsigned, std::vector<uint8_t> > pixelMaps;
    std::map<unsigned, std::vector<uint8_t> > flatPixelMaps;
		std::vector<unsigned> usedColours = {0u}; //0 - not used
		const std::function<bf::vec3d(const bf::vec3d&)> cFunc = [&](const bf::vec3d& p) {return c(p);};
		const std::function<bf::vec3d(const bf::vec3d&)> dcFunc = [&](const bf::vec3d& p) {return c(p);};
		std::vector<bf::DummySolid> debugDummySolids;
		std::vector<std::unique_ptr<bf::EquidistanceSurface> > surfaces;
		std::vector<bf::IntersectionObject*> intersections, flatIntersections;
		std::vector<bf::vec3d> createPathForSurface(const bf::Object& o, const std::vector<uint8_t>& colorMap, uint8_t color, double paramDist) const;
		std::vector<bf::vec3d> createPathForIntersection(const bf::IntersectionObject& io, double dist, unsigned begin, unsigned end) const;
		std::vector<bf::vec3d> createFlatBase(uint8_t color) const;
		bool createPixelMap(unsigned index); //false if pixel map coul not be created
		std::vector<bf::vec3d> generateExactPath(unsigned objIndex, uint8_t color, int diff, bool isXMove, int move=0, double normPerc=1.0, bool is4=false) const;
		void setDebugTextureIndex(int surface);
		void finishToEdge(bf::IntersectionObject &inter, bool is4=false);
		void saveIntersectionsToFile();
	public:
		bf::vec3d c(const bf::vec3d& p) const; //conversion of point from screen space to mulling space
		bf::vec3d dc(const bf::vec3d& p) const; //conversion of point from mulling space to screen space
        double getMPCScale() const {return scale;}
		bf::vec2d toSurfaceSpace(const bf::vec2d& pos) const {return bf::vec2d(c({pos, 0.0})/150.0)+bf::vec2d(0.5,0.5);}
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
		void findIntersections();
			//true if success
	void createApproximateMullingPath();
	void createFlatMullingPath();
	void createExactMullingPath();
	[[nodiscard]] bool isIntersectable() const override {return true;}
	[[nodiscard]] vec3d parameterFunction(double u, double v) const override;
	[[nodiscard]] vec3d parameterGradientU(double u, double v) const override;
	[[nodiscard]] vec3d parameterGradientV(double u, double v) const override;
	[[nodiscard]] vec3d parameterHesseUU(double u, double v) const override;
	[[nodiscard]] vec3d parameterHesseUV(double u, double v) const override;
	[[nodiscard]] vec3d parameterHesseVV(double u, double v) const override;
	bf::vec3d getPositionMovedByNormal(bf::IntersectionObject* obj, const glm::vec4& part) const;
};

} // bf

#endif //MULLINGPATHCREATOR_H
