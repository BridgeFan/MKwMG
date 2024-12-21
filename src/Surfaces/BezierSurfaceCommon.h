//
// Created by kamil-hp on 16.05.23.
//

#ifndef MG1_ZAD2_BEZIERSURFACECOMMON_H
#define MG1_ZAD2_BEZIERSURFACECOMMON_H

#include "Object/Object.h"
#include "Object/ObjectArrayListener.h"
#include <string>
#include <array>
#include <vector>
#include "BezierSurfaceSegment.h"

namespace bf {
    class ObjectArray;
    class Cursor;
    using pArray = std::array<unsigned, 16>;

    class BezierSurfaceCommon: public bf::Solid, public bf::ObjectArrayListener {
        friend bool loadFromFile(bf::ObjectArray &objectArray, const std::string &path);
    protected:
        static int _index;
        int activeIndex=-1;
        const bf::Cursor& cursor;
        bool isPolygonVisible=false, isSurfaceVisible=true;
        bool isC2 = false;
		std::tuple<int, int, double, double> setParameters(double u, double v) const;
    public:
        std::vector<std::vector<bf::BezierSurfaceSegment> > segments;
        virtual ~BezierSurfaceCommon() override;
        bool isWrappedX=false, isWrappedY=false;
        glm::vec<2,int> segs={3,3};
        glm::vec<2,int> samples;
        //std::vector<std::vector<pArray> > pointIndices;
        bool postInit() override;
        void surfacePostInit(std::vector<std::vector<pArray> >&& pointIndices);
        void draw(const ShaderArray &shader) const override;
        void ObjectGui() override;
        bool isMovable() const override;
        ShaderType getShaderType() const override;
        explicit BezierSurfaceCommon(bf::ObjectArray &objectArray, const std::string &objName, const bf::Cursor& c);
        explicit BezierSurfaceCommon(bf::ObjectArray &objectArray, const bf::Cursor& c);
        void onRemoveObject(unsigned int index) override;
        void onMoveObject(unsigned int index) override;
        virtual std::vector<std::vector<pArray>> generatePoints(const glm::vec2& totalSize) = 0;
        void recalculateSegments(unsigned int index);
        virtual void initSegments(std::vector<std::vector<std::string> >&& segmentNames,
            std::vector<std::vector<glm::vec<2,int> > >&& segmentSamples,
            std::vector<std::vector<pArray> >&& pointIndices);
		void onMergePoints(int p1, int p2) override;
		[[nodiscard]] bool isIntersectable() const override {return true;}
		[[nodiscard]] bf::vec2d getParameterMin() const override {return {0.,0.};}
		[[nodiscard]] bf::vec2d getParameterMax() const override {return {segs.x,segs.y};}
		[[nodiscard]] virtual bool parameterWrappingU() const override {return isWrappedX;}
		[[nodiscard]] virtual bool parameterWrappingV() const override {return isWrappedY;}
    	std::pair<glm::vec3, glm::vec3> getObjectRange() const override; //[min, max]
	};
}


#endif //MG1_ZAD2_BEZIERSURFACECOMMON_H
