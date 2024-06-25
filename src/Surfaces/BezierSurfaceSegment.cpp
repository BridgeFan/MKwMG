//
// Created by kamil-hp on 09.05.23.
//

#include <algorithm>
#include <GL/glew.h>
#include "BezierSurfaceSegment.h"
#include "Object/ObjectArray.h"
#include "Shader/ShaderArray.h"
#include <OpenGLUtil.h>

int bf::BezierSurfaceSegment::_index = 0;

void bf::BezierSurfaceSegment::initGL(const bf::ObjectArray &objectArray) {
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


void bf::BezierSurfaceSegment::segmentDraw(const bf::ShaderArray &shaderArray, bool isLineDrawn, bool isSurfaceDrawn, bool isChosen, unsigned ax, unsigned ay) const {
    if(indices.empty() || vertices.empty())
        return;
    if(shaderArray.getActiveIndex() == BasicShader && isLineDrawn) {
        if(isChosen)
            shaderArray.setColor(255, 128, 255);
        else
            shaderArray.setColor(64, 64, 128);
        //function assumes set projection and view matrices
        glBindVertexArray(VAO);
        shaderArray.setUniform("model", glm::mat4(1.f));
        glDrawElements(GL_LINES, 48, GL_UNSIGNED_INT,   // type
                       reinterpret_cast<void*>(16*sizeof(unsigned))           // element array buffer offset
        );
    }
    else if(shaderArray.getActiveIndex() == (isC2 ? BezierSurfaceShader2 : BezierSurfaceShader0) &&
        isSurfaceDrawn) {
		shaderArray.setUniform("add", glm::vec2(ax, ay));
        static bool wasChosen=false;
        if(isChosen) {
            shaderArray.setColor(128, 128, 255);
            wasChosen=true;
        }
        else if(wasChosen) {
            shaderArray.setColor(255, 128, 0);
            wasChosen=false;
        }
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

void bf::BezierSurfaceSegment::onPointMove(const bf::ObjectArray &objectArray, unsigned int index) {
    auto it = std::find(pointIndices.begin(),pointIndices.end(), index);
    if(it!=pointIndices.end()) {
        unsigned itIndex = it - pointIndices.begin();
        vertices[itIndex]=objectArray[index].getPosition();
        bf::gl::namedBufferSubData(VBO, vertices, itIndex, 1);
    }
}

void bf::BezierSurfaceSegment::onPointRemove(unsigned int index) {
    //point of Bezier Surface Segment cannot be removed
    for(auto& i: pointIndices) {
        if(i>index)
            i--;
    }
}

bf::BezierSurfaceSegment::BezierSurfaceSegment(bf::BezierSurfaceSegment &&solid) noexcept : Solid(std::move(solid)) {
    swapSegments(*this, solid);
	tmpIndices=std::move(solid.tmpIndices);
    isDynamic=true;
}

bf::BezierSurfaceSegment::BezierSurfaceSegment(bool c2) : Solid("Segment "+std::to_string(_index)), isC2(c2) {
    isDynamic=true;
    _index++;
}

void bf::BezierSurfaceSegment::swapSegments(bf::BezierSurfaceSegment &a, bf::BezierSurfaceSegment &b) {
    std::swap(a.samples, b.samples);
    std::swap(a.pointIndices, b.pointIndices);
    std::swap(a.isC2, b.isC2);
}

bf::BezierSurfaceSegment &bf::BezierSurfaceSegment::operator=(bf::BezierSurfaceSegment && segment) noexcept {
    swapSolids(*this, segment);
    swapSegments(*this, segment);
	tmpIndices = std::move(segment.tmpIndices);
    segment.VAO=segment.VBO=segment.IBO=UINT_MAX;
    return *this;
}
