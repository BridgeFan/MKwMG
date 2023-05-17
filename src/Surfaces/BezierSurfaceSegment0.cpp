//
// Created by kamil-hp on 09.05.23.
//

#include <algorithm>
#include <GL/glew.h>
#include "BezierSurfaceSegment0.h"
#include "Object/ObjectArray.h"
#include "Shader/ShaderArray.h"
#include <OpenGLUtil.h>

void bf::BezierSurfaceSegment0::initGL(const bf::ObjectArray &objectArray) {
    vertices.clear();
    indices.clear();
    for(auto i: pointIndices) {
        vertices.emplace_back(objectArray[i].getPosition());
    }
    //first part has size 16
    for(int i=0;i<16;i++)
        indices.push_back(i);
    //second part has size
    for(int i=0;i<3;i++) {
        for(int j=0;j<3;j++) {
            indices.push_back(i*4+j);
            indices.push_back(i*4+j+4);
            indices.push_back(i*4+j);
            indices.push_back(i*4+j+1);
        }
    }
    for(int i=0;i<3;i++) {
        indices.push_back(4*i+3);
        indices.push_back(4*i+7);
        indices.push_back(12+i);
        indices.push_back(13+i);
    }
    setBuffers();
}


void bf::BezierSurfaceSegment0::segmentDraw(const bf::ShaderArray &shaderArray, bool isLineDrawn, bool isSurfaceDrawn) const {
    if(indices.empty() || vertices.empty())
        return;
    if(shaderArray.getActiveIndex() == BasicShader && isLineDrawn) {
        shaderArray.setColor(64, 64, 128);
        //function assumes set projection and view matrices
        glBindVertexArray(VAO);
        glDrawElements(GL_LINES, 48, GL_UNSIGNED_INT,   // type
                       reinterpret_cast<void*>(16*sizeof(unsigned))           // element array buffer offset
        );
    }
    else if(shaderArray.getActiveIndex() == BezierSurfaceShader && isSurfaceDrawn) {
        glPatchParameteri( GL_PATCH_VERTICES, 16);
        const auto& shader = shaderArray.getActiveShader();
        shader.setInt("SegmentsX",samples.x);
        shader.setInt("SegmentsY",samples.y);
        shader.setVec2("DivisionBegin",{.0f,.0f});
        shader.setVec2("DivisionEnd",{1.f,1.f});
        glBindVertexArray(VAO);
        glDrawElements(GL_PATCHES, 16, GL_UNSIGNED_INT,   // type
                       0           // element array buffer offset
        );
    }
}

void bf::BezierSurfaceSegment0::onPointMove(const bf::ObjectArray &objectArray, unsigned int index) {
    auto it = std::find(pointIndices.begin(),pointIndices.end(), index);
    if(it!=pointIndices.end()) {
        auto itIndex = static_cast<unsigned>(it - pointIndices.begin());
        vertices[itIndex]=objectArray[index].getPosition();
        bf::gl::namedBufferSubData(VBO, vertices, itIndex, 1);
    }
}

void bf::BezierSurfaceSegment0::onPointRemove(unsigned int index) {
    //point of Bezier Surface Segment cannot be removed
    for(auto& i: pointIndices) {
        if(i>index)
            i--;
    }
}

bf::BezierSurfaceSegment0::BezierSurfaceSegment0(bf::BezierSurfaceSegment0 &&solid) noexcept : Solid(std::move(solid)) {
    swapSegments(*this, solid);
    isDynamic=true;
}

bf::BezierSurfaceSegment0::BezierSurfaceSegment0() : Solid() {
    isDynamic=true;
}

void bf::BezierSurfaceSegment0::swapSegments(bf::BezierSurfaceSegment0 &a, bf::BezierSurfaceSegment0 &b) {
    std::swap(a.samples, b.samples);
    std::swap(a.pointIndices, b.pointIndices);
}

bf::BezierSurfaceSegment0 &bf::BezierSurfaceSegment0::operator=(bf::BezierSurfaceSegment0 && segment) noexcept {
    swapSolids(*this, segment);
    swapSegments(*this, segment);
    segment.VAO=segment.VBO=segment.IBO=UINT_MAX;
    return *this;
}
