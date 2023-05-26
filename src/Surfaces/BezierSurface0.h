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
        BezierSurface0(ObjectArray &objectArray, const std::string &objName, const Cursor &c);
        BezierSurface0(ObjectArray &objectArray, const Cursor &c);
        virtual std::vector<std::vector<pArray>> generatePoints(const glm::vec2& totalSize) override;
    };
}


#endif //MG1_ZAD2_BEZIERSURFACE0_H
