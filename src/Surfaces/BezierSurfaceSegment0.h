#pragma once
//
// Created by kamil-hp on 09.05.23.
//

#ifndef MG1_ZAD2_BEZIERSURFACESEGMENT0_H
#define MG1_ZAD2_BEZIERSURFACESEGMENT0_H

#include <array>
#include <string>
#include <glm/detail/type_vec2.hpp>
#include "Solids/Solid.h"

namespace bf {
    class ObjectArray;
    class ShaderArray;
    class BezierSurfaceSegment0: public bf::Solid {
        friend bool loadFromFile(bf::ObjectArray &objectArray, const std::string &path);
        void swapSegments(BezierSurfaceSegment0& a, BezierSurfaceSegment0& b);
    public:
        glm::vec<2,int> samples;
        std::array<unsigned, 16> pointIndices;
        void initGL(const bf::ObjectArray &objectArray);
        void onPointMove(const bf::ObjectArray &objectArray, unsigned index);
        void onPointRemove(unsigned index);
        void segmentDraw(const bf::ShaderArray &shader, bool isLineDrawn, bool isSurfaceDrawn) const;
        BezierSurfaceSegment0(Solid &solid)=delete;
        explicit BezierSurfaceSegment0(BezierSurfaceSegment0 &&solid) noexcept;
        explicit BezierSurfaceSegment0();
        BezierSurfaceSegment0& operator=(BezierSurfaceSegment0&)=delete;
        BezierSurfaceSegment0& operator=(BezierSurfaceSegment0&&) noexcept;
    };
}


#endif //MG1_ZAD2_BEZIERSURFACESEGMENT0_H
