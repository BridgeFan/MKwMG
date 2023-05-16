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
#include "BezierSurfaceSegment0.h"

namespace bf {
    class ObjectArray;
    class Cursor;
    typedef std::array<unsigned, 16> pArray;

    class BezierSurfaceCommon: public bf::Object, public bf::ObjectArrayListener {
        friend bool loadFromFile(bf::ObjectArray &objectArray, const std::string &path);
    protected:
        static int _index;
        const bf::Cursor& cursor;
        bool isPolygonVisible=false, isSurfaceVisible=true;
    public:
        std::vector<std::vector<bf::BezierSurfaceSegment0> > segments;
        virtual ~BezierSurfaceCommon() override;
        bool isWrappedX=false, isWrappedY=false;
        glm::vec<2,int> segs={3,3};
        glm::vec<2,int> samples;
        std::vector<std::vector<pArray> > pointIndices;
        void postInit() override;
        void draw(const ShaderArray &shader) const override;
        void ObjectGui() override;
        bool isMovable() const override;
        ShaderType getShaderType() const override;
        explicit BezierSurfaceCommon(bf::ObjectArray &objectArray, const std::string &objName, const bf::Cursor& c);
        explicit BezierSurfaceCommon(bf::ObjectArray &objectArray, const bf::Cursor& c);
        void onRemoveObject(unsigned int index) override;
        void onMoveObject(unsigned int index) override;
        virtual void recalculateSegments(unsigned int index) = 0;
        virtual void initSegments(std::vector<std::vector<std::string> >&& segmentNames,
            std::vector<std::vector<glm::vec<2,int> > >&& segmentSamples) = 0;
    };
}


#endif //MG1_ZAD2_BEZIERSURFACECOMMON_H
