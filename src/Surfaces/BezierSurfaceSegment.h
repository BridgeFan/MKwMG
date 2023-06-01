#pragma once
//
// Created by kamil-hp on 09.05.23.
//

#ifndef MG1_ZAD2_BEZIERSURFACESEGMENT0_H
#define MG1_ZAD2_BEZIERSURFACESEGMENT0_H

#include <array>
#include <string>
#include <glm/vec2.hpp>
#include "Solids/Solid.h"

namespace bf {
    class ObjectArray;
    struct ShaderArray;
    class BezierSurfaceSegment: public bf::Solid {
        friend bool loadFromFile(bf::ObjectArray &objectArray, const std::string &path);
        static int _index;
        void swapSegments(BezierSurfaceSegment& a, BezierSurfaceSegment& b);
        bool isC2=false;
    public:
        glm::vec<2,int> samples;
        std::array<unsigned, 16> pointIndices;
        void initGL(const bf::ObjectArray &objectArray);
        void onPointMove(const bf::ObjectArray &objectArray, unsigned index);
        void onPointRemove(unsigned index);
        void segmentDraw(const bf::ShaderArray &shader, bool isLineDrawn, bool isSurfaceDrawn, bool isChosen) const;
        BezierSurfaceSegment(Solid &solid)=delete;
        BezierSurfaceSegment(BezierSurfaceSegment &&solid) noexcept;
        explicit BezierSurfaceSegment(bool c2);
        BezierSurfaceSegment& operator=(BezierSurfaceSegment&)=delete;
        BezierSurfaceSegment& operator=(BezierSurfaceSegment&&) noexcept;
		void onMergePoints(int, int) override {}

    };
}


#endif //MG1_ZAD2_BEZIERSURFACESEGMENT0_H
