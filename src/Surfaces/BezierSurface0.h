#pragma once
//
// Created by kamil-hp on 06.05.23.
//

#ifndef MG1_ZAD2_BEZIERSURFACE0_H
#define MG1_ZAD2_BEZIERSURFACE0_H

#include "BezierSurfaceCommon.h"

namespace bf {
	class ObjectArray;
    class Cursor;

    class BezierSurface0: public bf::BezierSurfaceCommon {
        friend bool loadFromFile(bf::ObjectArray &objectArray, const std::string &path);
        static int _index;
    public:
        virtual void recalculateSegments(unsigned int index) override;
        virtual void initSegments(std::vector<std::vector<std::string> >&& segmentNames,
            std::vector<std::vector<glm::vec<2,int> > >&& segmentSamples) override;
        BezierSurface0(ObjectArray &objectArray, const std::string &objName, const Cursor &c);
        BezierSurface0(ObjectArray &objectArray, const Cursor &c);
    };
}


#endif //MG1_ZAD2_BEZIERSURFACE0_H
