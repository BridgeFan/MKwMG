//
// Created by kamil-hp on 18.04.23.
//

#include <GL/glew.h>
#include <optional>
#include "BasicBezier.h"
#include "Util.h"
#include "Scene.h"
#include "src/Shader/ShaderArray.h"
#include "ConfigState.h"

bf::BasicBezier::BasicBezier() {}

void bf::BasicBezier::recalculate(bool wasSizeChanged) {
	isDynamic=true;
	vertices.clear();
	for(auto & point : points) {
		vertices.emplace_back(point);
	}
    //update curve
    if(wasSizeChanged) {
        indices.clear();
		//basic topology
		for(unsigned i=0u;i<vertices.size();i++) {
			indices.push_back(i);
		}
		//tessellation topology
		for(unsigned i=0u;i<vertices.size();i++) {
			indices.push_back(i);
			if(i>0 && i%3==0 && i+1<vertices.size())
				indices.push_back(i);
		}
        setBuffers();
    }
    else if (VBO<UINT_MAX){
        glNamedBufferSubData(VBO, 0, vertices.size() * sizeof(Vertex), vertices.data());
    }
}

void bf::BasicBezier::draw(const bf::ShaderArray &shaderArray, const bf::Scene &,
                           const bf::ConfigState& configState, bool isLineDrawn, bool isPointDrawn) const {
	auto& shader = shaderArray.getActiveShader();
	if(VAO==UINT_MAX)
		return;
	if(shaderArray.getActiveIndex()==BasicShader) {
		glBindVertexArray(VAO);
        shader.setVec3("color", 1.f,0.f,1.f);
		if(isLineDrawn) {
			glDrawElements(GL_LINE_STRIP, vertices.size(), GL_UNSIGNED_INT, 0);
		}
	}
    else if(shaderArray.getActiveIndex()==PointShader) {
        glBindVertexArray(VAO);
        shader.setVec3("color", 1.f,0.f,1.f);
        shader.setVec3("position", 0.f,0.f,0.f);
        if(isPointDrawn) {
            glDrawElements(GL_POINTS, vertices.size(), GL_UNSIGNED_INT, 0);
        }
    }
	else if(shaderArray.getActiveIndex()==BezierShader) {
        glBindVertexArray(VAO);
		glPatchParameteri( GL_PATCH_VERTICES, 4);
        shader.setInt("MinSegments", 1);
        shader.setInt("MaxSegments", configState.totalDivision/configState.divisionNum);
        shader.setInt("ScreenWidth", configState.screenWidth);
        shader.setInt("ScreenHeight", configState.screenHeight);
        int vertSize = static_cast<int>(vertices.size());
        int fullSegments = (vertSize-1)/3;
        int restLength = (vertSize-1)%3+1;
        auto dNum = static_cast<float>(configState.divisionNum);
        for(int i=0;i<configState.divisionNum;i++) {
            shader.setFloat("DivisionBegin", static_cast<float>(i)/dNum);
            shader.setFloat("DivisionEnd", static_cast<float>(i+1)/dNum);
            shader.setInt("BezierNum", 4);
            glDrawElements(GL_PATCHES, 4 * fullSegments, GL_UNSIGNED_INT,
                           reinterpret_cast<void *>(vertSize * sizeof(GLuint)));
            if (restLength > 1) {
                shader.setInt("BezierNum", restLength);
                glDrawElements(GL_PATCHES, 4, GL_UNSIGNED_INT,
                               reinterpret_cast<void *>((vertSize + 4 * fullSegments) * sizeof(GLuint)));
            }
        }
	}
}

std::vector<glm::vec3> bf::bezier2ToBezier0(const std::vector<glm::vec3> &points) {
	std::vector<glm::vec3> bezier;
	if(points.size()>3) {
		bezier.resize(3*points.size()-8);
		for (int i = 1; i < static_cast<int>(points.size()) - 2; i++) {
			bezier[3*i-2]=lerp(points[i],points[i+1],1.f/3.f);
			bezier[3*i-1]=lerp(points[i],points[i+1],2.f/3.f);
			if(i==1u)
				bezier[0]=lerp(lerp(points[0],points[1],2.f/3.f),bezier[1],.5f);
			else
				bezier[3*i-3]=lerp(bezier[3*i-4],bezier[3*i-2],.5f);
		}
		bezier[3*points.size()-9]=
				lerp(bezier[3*points.size()-10],
					 lerp(points[points.size()-2],points[points.size()-1],1.f/3.f),
					 .5f);
	}
	return bezier;
}

std::optional<glm::vec3> getIntersection(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& r1, const glm::vec3& r2) {
    //calculate for XY plane
    if(almostEqual((p1.x-p2.x)*(r1.y-r2.y)-(p1.y-p2.y)*(r1.x-r2.x),.0f)) //colinear
        return std::nullopt;
    auto sols = tridiagonalMatrixAlgorithm<float>({p2.y-p1.y},{p2.x-p1.x,r1.y-r2.y},{r1.x-r2.x},{r1.x-p1.x,r1.y-p1.y});
    return p1+sols[0]*(p2-p1);
}

std::vector<glm::vec3> bf::bezier0ToBezier2(const std::vector<glm::vec3> &p) {
	if(p.size()%3!=1 || p.size()<4)
		return {};
	std::vector<glm::vec3> retPoints(p.size()/3+3);
    for(unsigned i=1u;i<retPoints.size()-1;i++) {
        auto inter = getIntersection(p[3*i-2],p[3*i-1],p[3*i+1],p[3*i+2]);
        if(inter)
            retPoints[i+1]=*inter;
        else
            retPoints[i+1]=p[3*i];
    }
    retPoints[1]=lerp(p[1],p[2],-1.f);
    retPoints[0]=3.f*(p[0]-p[1])+retPoints[2];
    retPoints[retPoints.size()-2]=lerp(p[p.size()-2],p[p.size()-3],-1.f);
    retPoints[retPoints.size()-1]=3.f*(p[p.size()-1]-p[p.size()-2])+retPoints[retPoints.size()-2];
	return retPoints;
}
